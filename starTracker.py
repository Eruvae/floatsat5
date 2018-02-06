import cv2
import math
import numpy
import time

# VEHICLE CALIBRATION
CAMERA_VEHICLE_DX = 0 # distance that camera is to the 'left' of vehicle, in mm
CAMERA_VEHICLE_DY = 200  # distance that camera is in 'front' of vehicle, in mm
CAM_FOV_MM_X = 270       # effective camera FOV width on table, in mm
CAM_FOV_MM_Y = 230       # effective camera FOV height on table, in mm

# ST algorithm calibration
MAX_ATTEMPTS = 5  # number of different stars algorithm will try before giving up
THRESHOLD = 5.0   # max angular error: lower values cause less matches but higher confidence

# Star detection tweaking
# mess with these if video shows stars not being detected,
# but first try brightness and contrast which are more important
MIN_BLOB_AREA = 5.0  
MAX_BLOB_AREA = 40.0 
MIN_DIST_BETWEEN = 2.0
MIN_CONVEXITY = 0.9 

# path to catalog file
CATALOG_PATH     = "./catalog.txt"

# Classes
class Star(object):
    def __init__(self):
        self.id = 0
        self.x = 0
        self.y = 0
        self.score = 0
        self.n_dists = []
        self.neighbour_1 = 0
        self.neighbour_2 = 0
        self.neighbour_3 = 0
        self.neighbour_4 = 0
        self.neighbour_5 = 0
        self.neighbour_6 = 0
        self.t1_a = 0
        self.t1_b = 0
        self.t1_c = 0
        self.t2_a = 0
        self.t2_b = 0
        self.t2_c = 0
        self.t3_a = 0
        self.t3_b = 0
        self.t3_c = 0
        self.t4_a = 0
        self.t4_b = 0
        self.t4_c = 0
        self.t5_a = 0
        self.t5_b = 0
        self.t5_c = 0


# detects stars in image
# returns list of stars and new image with stars circled
def detect_stars(image):

    # create blob detector
    params = cv2.SimpleBlobDetector_Params()
    params.minArea = MIN_BLOB_AREA
    params.maxArea = MAX_BLOB_AREA
    params.minDistBetweenBlobs = MIN_DIST_BETWEEN
    params.minConvexity = MIN_CONVEXITY
    detector = cv2.SimpleBlobDetector_create(params)
    keypoints = detector.detect(image)

    # draw keypoint circles onto image
    image = cv2.drawKeypoints(image, keypoints, numpy.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

    # sort keypoints by x, then y values
    keypoints = sorted(keypoints, key=lambda x: (x.pt[0], x.pt[1]))
    
    # create list of Star()s
    stars = []
    for i in range(len(keypoints)):
        star = Star()
        star.id = i
        star.x = numpy.round(keypoints[i].pt[0], 1)
        star.y = numpy.round(keypoints[i].pt[1], 1)
        stars.append(star)
    return (stars, image)


# get planar angles of one triangle
def get_planar_angles(target_star, neighbour_1, neighbour_2, decimals):
    def abs_limit(val, limit):
        if(val > limit):
            val = limit
        elif(val < (-1.0*limit)):
            val = -1.0*limit
        return val
    
    # calculate angles via law of cosines
    a = math.sqrt(((neighbour_1.x - target_star.x)**2) + ((neighbour_1.y - target_star.y)**2))
    b = math.sqrt(((target_star.x - neighbour_2.x)**2) + ((target_star.y - neighbour_2.y)**2))
    c = math.sqrt(((neighbour_1.x - neighbour_2.x)**2) + ((neighbour_1.y - neighbour_2.y)**2))
    alpha = math.acos(abs_limit(((a**2) - (c**2) - (b**2)) / (-2.0*b*c if -2.0*b*c != 0 else 0.0000001), 1))
    beta  = math.acos(abs_limit(((b**2) - (a**2) - (c**2)) / (-2.0*a*c if -2.0*a*c != 0 else 0.0000001), 1))
    gamma = math.acos(abs_limit(((c**2) - (a**2) - (b**2)) / (-2.0*a*b if -2.0*a*b != 0 else 0.0000001), 1))

    # return in degrees and rounded to specified accuracy
    return (numpy.round(math.degrees(alpha), decimals),
            numpy.round(math.degrees(beta), decimals),
            numpy.round(math.degrees(gamma), decimals))


# compute interior planar angles for one star
# uses 5 triangles from star-n1-n2, star-n1-n3 to star-n1-6
def compute_planar_angles(star, stars):
    star.t1_a, star.t1_b, star.t1_c = get_planar_angles(star, stars[star.neighbour_1], stars[star.neighbour_2], 2)
    star.t2_a, star.t2_b, star.t2_c = get_planar_angles(star, stars[star.neighbour_1], stars[star.neighbour_3], 2)
    star.t3_a, star.t3_b, star.t3_c = get_planar_angles(star, stars[star.neighbour_1], stars[star.neighbour_4], 2)
    star.t4_a, star.t4_b, star.t4_c = get_planar_angles(star, stars[star.neighbour_1], stars[star.neighbour_5], 2)
    star.t5_a, star.t5_b, star.t5_c = get_planar_angles(star, stars[star.neighbour_1], stars[star.neighbour_6], 2)
    return star


# find closest neighbours for each star
def get_neighbours(stars):
    for i in range(len(stars)):
        n_dists = [10000, 10000, 10000, 10000, 10000, 10000]
        for j in range(len(stars)):
            dx = stars[j].x - stars[i].x
            dy = stars[j].y - stars[i].y
            if (abs(dx) > 200 or abs(dy) > 200):
                # optimisation: skip stars further than 200mm 
                continue
            d = math.sqrt((dx**2) + (dy**2))
            if j != i:
                if (d < n_dists[0]):
                    n_dists.insert(0, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[i].neighbour_5
                    stars[i].neighbour_5 = stars[i].neighbour_4
                    stars[i].neighbour_4 = stars[i].neighbour_3
                    stars[i].neighbour_3 = stars[i].neighbour_2
                    stars[i].neighbour_2 = stars[i].neighbour_1
                    stars[i].neighbour_1 = stars[j].id
                elif (d < n_dists[1]):
                    n_dists.insert(1, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[i].neighbour_5
                    stars[i].neighbour_5 = stars[i].neighbour_4
                    stars[i].neighbour_4 = stars[i].neighbour_3
                    stars[i].neighbour_3 = stars[i].neighbour_2
                    stars[i].neighbour_2 = stars[j].id
                elif (d < n_dists[2]):
                    n_dists.insert(2, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[i].neighbour_5
                    stars[i].neighbour_5 = stars[i].neighbour_4
                    stars[i].neighbour_4 = stars[i].neighbour_3
                    stars[i].neighbour_3 = stars[j].id
                elif (d < n_dists[3]):
                    n_dists.insert(3, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[i].neighbour_5
                    stars[i].neighbour_5 = stars[i].neighbour_4
                    stars[i].neighbour_4 = stars[j].id
                elif (d < n_dists[4]):
                    n_dists.insert(4, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[i].neighbour_5
                    stars[i].neighbour_5 = stars[j].id
                elif (d < n_dists[5]):
                    n_dists.insert(5, d)
                    n_dists.pop(6)
                    stars[i].neighbour_6 = stars[j].id
                stars[i].n_dists = n_dists
    return stars


# ranks stars detection potential,
# ie how well they are likely to find a match with catalog
# stars at edge of image are bad because missing neighbours
# stars with similarly close neighbours are bad because they could be switched
def rank_by_detection_potential(stars, camera_width, camera_height):
    for star in stars:
        # Assign each star a detection potential "score", lower is better
        star.score = 0
        
        # Compute centre distance
        d_centre = math.sqrt(((star.x-(camera_width/2.0))**2) + ((star.y-(camera_height/2.0))**2))

        # Add distance to score: the further the star from centre the worse
        star.score += d_centre
        
        # Subtract score if neighbouring stars are too close to each other
        threshold = 5
        for i in range(5):
            # Is distance between neighbour i and neibhour i+1 less than threshold?
            if ((star.n_dists[i+1] - star.n_dists[i]) < threshold):
                # Increase score proportionally: d1<>2 more important than d4<>5 etc
                star.score += (5 + (5 * (5-i)))
                
    # return stars sorted by lowest score
    return sorted(stars, key=lambda x: x.score)      


# find given star in catalog by comparing triangle planar angles
# for a successful match all angular errors must be below certain threshold
# given multiple matches the match with lowest total error is returned
# returns None if no match is found
# comment out later ifs to reduce number of triangles used
def identify_star(star, catalog):
    best_guess = None
    # calculate differences between stars planar angles and each catalog entry
    for entry in catalog:
        threshold = 5.0
        lowest_error = 10000
        # calculate triangle1 errors and check if above threshold
        e_t1_a, e_t1_b, e_t1_c = abs(entry.t1_a-star.t1_a), abs(entry.t1_b-star.t1_b), abs(entry.t1_c-star.t1_c)
        if ((e_t1_a > threshold) or (e_t1_b > threshold) or (e_t1_c > threshold)):
            # first triangle angles dont match
            # -> go to next catalog entry
            continue
        # calculate triangle2 errors and check if above threshold
        e_t2_a, e_t2_b, e_t2_c = abs(entry.t2_a-star.t2_a), abs(entry.t2_b-star.t2_b), abs(entry.t2_c-star.t2_c)
        if ((e_t2_a > threshold) or (e_t2_b > threshold) or (e_t2_c > threshold)):
            #print("\n\ntriangle 2")
            continue
        # calculate triangle3 errors and check if above threshold
        e_t3_a, e_t3_b, e_t3_c = abs(entry.t3_a-star.t3_a), abs(entry.t3_b-star.t3_b), abs(entry.t3_c-star.t3_c)
        if ((e_t3_a > threshold) or (e_t3_b > threshold) or (e_t3_c > threshold)):
            continue
        # calculate triangle4 errors and check if above threshold
        e_t4_a, e_t4_b, e_t4_c = abs(entry.t4_a-star.t4_a), abs(entry.t4_b-star.t4_b), abs(entry.t4_c-star.t4_c)
        if ((e_t4_a > threshold) or (e_t4_b > threshold) or (e_t4_c > threshold)):
            continue
        # calculate triangle5 errors and check if above threshold
        e_t5_a, e_t5_b, e_t5_c = abs(entry.t5_a-star.t5_a), abs(entry.t5_b-star.t5_b), abs(entry.t5_c-star.t5_c)
        #if ((e_t5_a > threshold) or (e_t5_b > threshold) or (e_t5_c > threshold)):
            #continue
        # Compute average of all errors
        e_avg = numpy.average([e_t1_a,e_t1_b,e_t1_c,e_t2_a,e_t2_b,e_t2_c,e_t3_a,e_t3_b,e_t3_c,e_t4_a,e_t4_b,e_t4_c,e_t5_a,e_t5_b,e_t5_c])
        e_min = min(e_t1_a,e_t1_b,e_t1_c,e_t2_a,e_t2_b,e_t2_c,e_t3_a,e_t3_b,e_t3_c,e_t4_a,e_t4_b,e_t4_c,e_t5_a,e_t5_b,e_t5_c)
        e_max = max(e_t1_a,e_t1_b,e_t1_c,e_t2_a,e_t2_b,e_t2_c,e_t3_a,e_t3_b,e_t3_c,e_t4_a,e_t4_b,e_t4_c,e_t5_a,e_t5_b,e_t5_c)
        # Does this match have better overall average error than previous best?
        if (e_avg < lowest_error):
            # New best guess
            best_guess = entry
    if best_guess:
        print("Found match: star identified as catalog star %d, with e_avg=%3.2f, e_min=%3.2f and e_max=%3.2f" % (best_guess.id, e_avg, e_min, e_max))
    return best_guess


# Main star tracking method
def track_stars(image, catalog, show_video, debug):

    start = time.time()

    # get camera resolution from image size
    camera_width, camera_height  = len(image[0]), len(image)
    print(camera_width)
    print(camera_height)

    # Invert image to black on white for blob detection
    image = cv2.bitwise_not(image)

    # Detect stars
    print("detecting stars...")
    stars, image = detect_stars(image)
    print("found %d stars in image" % len(stars))

    # Cant do ST with less than 7 stars: target + 6 neighbours
    if len(stars) < 7:
        print("Aborting, found less than 7 stars required for algorithm")
        return None
    
    # Determine closest neighbours
    print("finding neighbours...")
    stars = get_neighbours(stars)

    # Rank stars by detection potential
    print("ranking stars...")
    ranked_stars = rank_by_detection_potential(stars, camera_width, camera_height)

    # Process stars in order of detection potential
    camera_star = None
    found = False
    for i in range(len(ranked_stars)):

        print("Trying to find catalog match for star %d" % (i+1))
        camera_star = ranked_stars[i]
       
        # compute planar angles
        camera_star = compute_planar_angles(camera_star, stars)

        # try to identify star
        catalog_star = identify_star(camera_star, catalog)

        # Did we successfully identify star?
        if catalog_star:
            found = True
            break

        # Have we exhausted our number of attempts?
        if (i == (MAX_ATTEMPTS -1)):
            # give up, for now...
            print("Failed to find a catalog match for the first %d stars in ranked list" % MAX_ATTEMPTS)
            break

    # Return None as coords if we didnt find a match
    if not found:
        star_coordinates = None

    else:
        # Compute heading from difference in target_star <> neighbour_star_1 vector
        theta_cam = math.atan2((stars[camera_star.neighbour_1].y - camera_star.y), stars[camera_star.neighbour_1].x - camera_star.x)
        theta_SMD = math.atan2(catalog[catalog_star.neighbour_1].y - catalog_star.y, catalog[catalog_star.neighbour_1].x - catalog_star.x)
        theta = float(theta_cam - theta_SMD)

        # Convert heading to degrees
        theta = theta * 180.0 / math.pi

        # Calibrate for chosen coordinate system, theta positive anticlockwise from SMD y-axis
        theta -= 180

        # normalise theta to range of -180 to +180
        if theta > 180:
            theta -= 360
        if theta < -180:
            theta += 360

        # Get effective offsets between target star and vehicle centre
        # sum of vehicle <-> camera centre and camera centre <-> camera star
        dx = CAMERA_VEHICLE_DX + ((camera_star.x - (camera_width/2)) * CAM_FOV_MM_X / camera_width)
        dy = CAMERA_VEHICLE_DY - ((camera_star.y - (camera_height/2)) * CAM_FOV_MM_Y / camera_height)

        # Transform from camera frame to vehicle frame
        # using vector addition and a rotation matrix, SD for the win
        dxcostheta = dx * math.cos(theta*math.pi/180)
        dysintheta = dy * math.sin(theta*math.pi/180)
        dxsintheta = dx * math.sin(theta*math.pi/180)
        dycostheta = dy * math.cos(theta*math.pi/180)
        # shoutout to Martin for helping in getting this right
        x_vehicle = catalog_star.x + dxcostheta - dysintheta
        y_vehicle = catalog_star.y - dxsintheta - dycostheta

        if (debug == True):
            print("catalog star position mm : x=%s y=%s" % (catalog_star.x, catalog_star.y))
            print("camera star position px  : x=%s y=%s" % (camera_star.x, camera_star.y))
            print("vehicle position mm      : x=%s y=%s" % (x_vehicle, y_vehicle))
            print("heading = %s" % theta)

        # Return x, y and theta of the vehicle
        star_coordinates = {
        "x": x_vehicle,
        "y": y_vehicle,
        "theta": theta
        }

    # show live video
    if show_video:
        # print detected star no and x and y on screen
        if found: 
            cv2.putText(image, "star %d x=%.0f y=%.0f theta=%0.f" % (catalog_star.id, catalog_star.x, catalog_star.y, theta), (10, 15), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,255))
            cv2.putText(image, "star %d x=%.0f y=%.0f theta=%0.f" % (catalog_star.id, x_vehicle, y_vehicle, theta), (10, 30), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,255))
        # Draw triangle
        if camera_star:
            cv2.line(image, (int(camera_star.x), int(camera_star.y)), (int(stars[camera_star.neighbour_1].x), int(stars[camera_star.neighbour_1].y)), (0, 0, 255), 1, 1)
            cv2.line(image, (int(camera_star.x), int(camera_star.y)), (int(stars[camera_star.neighbour_2].x), int(stars[camera_star.neighbour_2].y)), (0, 0, 255), 1, 1)
        # Draw star rank numbers onto image
        for i in range(min(MAX_ATTEMPTS, len(ranked_stars))):
            cv2.putText(image,str(i+1), (int(ranked_stars[i].x+5),int(ranked_stars[i].y)), cv2.FONT_HERSHEY_SIMPLEX, 0.3, 255)
        # Draw FPS onto image
        cv2.putText(image, "FPS %.2f" % (1.0/(time.time()-start)), (camera_width-90, camera_height-10), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,255))
        cv2.namedWindow('frame', cv2.WINDOW_NORMAL)
        cv2.resizeWindow('frame', 600, 480)
        cv2.imshow('frame', image)
        cv2.waitKey(1)

    return star_coordinates

# Read catalogs into memory
def read_catalog():
    catalog = []
    with open(CATALOG_PATH) as cat_file:
        lines = cat_file.readlines()
        # format is 'id x y n1 n2 n3 n4 n5 n6 t1a t1b t1c t2a t2c t2c t3a t3b t3c t4a t4b t4c t5a t5b t5c'
        for line in lines:
            entries = line.split(' ')
            star = Star()
            star.id = int(entries[0])
            star.x = float(entries[1])
            star.y = float(entries[2])
            star.neighbour_1 = int(entries[3])
            star.neighbour_2 = int(entries[4])
            star.neighbour_3 = int(entries[5])
            star.neighbour_4 = int(entries[6])
            star.neighbour_5 = int(entries[7])
            star.neighbour_6 = int(entries[8])
            star.t1_a = float(entries[9])
            star.t1_b = float(entries[10])
            star.t1_c = float(entries[11])
            star.t2_a = float(entries[12])
            star.t2_b = float(entries[13])
            star.t2_c = float(entries[14])
            star.t3_a = float(entries[15])
            star.t3_b = float(entries[16])
            star.t3_c = float(entries[17])
            star.t4_a = float(entries[18])
            star.t4_b = float(entries[19])
            star.t4_c = float(entries[20])
            star.t5_a = float(entries[21])
            star.t5_b = float(entries[22])
            star.t5_c = float(entries[23])
            catalog.append(star)
    return catalog

# debug code to print object attrs
#print(" ".join("%s: %s" % item for item in vars(object).items()))
 
