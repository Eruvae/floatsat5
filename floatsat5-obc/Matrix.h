#pragma once

#include <rodos.h>

template<size_t m, size_t n = m, typename T = double>
class Matrix
{
private:
	T data[m][n];
	void clearData();

	void swapRow(int i, int j);
	void divideRow(int i, T d);
	void subRow(int i, int j, T f = 1.0);

public:
	Matrix();
	Matrix(T **arr);
	Matrix(T *arr);
	Matrix(T arr[m][n]);
	Matrix(const Matrix<m, n, T> &r);
	~Matrix();

	static Matrix<m, n, T> identity_matrix();

	Matrix<m, n, T>& operator*= (const T &r);
	Matrix<m, n, T>& operator/= (const T &r);
	Matrix<m, n, T>& operator+= (const Matrix<m, n, T> &r);
	Matrix<m, n, T>& operator-= (const Matrix<m, n, T> &r);
	Matrix<m, n, T>& operator= (const Matrix<m, n, T> &r);

	T getDeterminant() const;
	T getLrDeterminant() const;
	Matrix<m, n, T> getLrGauss() const;
	Matrix<m, n, T> getGaussEliMat() const;
	Matrix<m, n, T> getInverse() const;
	Matrix<n, m, T> getTransposed() const;

	T* operator[] (const int row)
	{
		return data[row];
	}

	const T* const operator[] (const int row) const
	{
		return data[row];
	}
};

#if __cplusplus >= 201103L || _MSVC_LANG >= 201103L
template <size_t m, typename T = double>
using Vector = Matrix<m, 1, T>;
#endif

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::Matrix()
{
	clearData();
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::Matrix(T **arr) : Matrix()
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] = arr[i][j];
		}
	}
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::Matrix(T *arr) : Matrix()
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] = arr[i * n + j];
		}
	}
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::Matrix(T arr[m][n])
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] = arr[i][j];
		}
	}
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::Matrix(const Matrix<m, n, T> &r)
{
	*this = r;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>::~Matrix()
{
}

template<size_t m, size_t n, typename T>
void Matrix<m, n, T>::clearData()
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] = 0;
		}
	}
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> Matrix<m, n, T>::identity_matrix()
{
	Matrix<m, n, T> tmp;
	int k = m < n ? m : n;
	for (int i = 0; i < k; i++)
	{
		tmp[i][i] = 1;
	}
	return tmp;
}

template<size_t m, size_t n, typename T>
void Matrix<m, n, T>::swapRow(int i, int j)
{
	T tmp[n];
	memcpy(tmp, data[i], n * sizeof(T));
	memcpy(data[i], data[j], n * sizeof(T));
	memcpy(data[j], tmp, n * sizeof(T));
}

template<size_t m, size_t n, typename T>
void Matrix<m, n, T>::divideRow(int i, T d)
{
	for (int j = 0; j < n; j++)
		data[i][j] /= d;
}

template<size_t m, size_t n, typename T>
void Matrix<m, n, T>::subRow(int i, int j, T f)
{
	for (int k = 0; k < n; k++)
		data[i][k] -= f*data[j][k];
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>& Matrix<m, n, T>::operator*= (const T &r)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] *= r;
		}
	}
	return *this;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>& Matrix<m, n, T>::operator/= (const T &r)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] /= r;
		}
	}
	return *this;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>& Matrix<m, n, T>::operator+= (const Matrix<m, n, T> &r)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] += r[i][j];
		}
	}
	return *this;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>& Matrix<m, n, T>::operator-= (const Matrix<m, n, T> &r)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] -= r[i][j];
		}
	}
	return *this;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T>& Matrix<m, n, T>::operator= (const Matrix<m, n, T> &r)
{
	if (this == &r)
		return *this;

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			data[i][j] = r[i][j];
		}
	}
	return *this;
}

template<size_t m, size_t n, typename T>
T Matrix<m, n, T>::getDeterminant() const
{
	if (m == 2)
	{
		return data[0][0] * data[1][1] - data[0][1] * data[1][0];
	}
	else if (m == 3)
	{
		return data[0][0] * data[1][1] * data[2][2] + data[0][1] * data[1][2] * data[2][0] + data[0][2] * data[1][0] * data[2][1]
			- data[2][0] * data[1][1] * data[0][2] - data[2][1] * data[1][2] * data[0][0] - data[2][2] * data[1][0] * data[0][1];
	}
	else if (m > 3)
	{
		return getLrDeterminant();
	}
}

template<size_t m, size_t n, typename T>
T Matrix<m, n, T>::getLrDeterminant() const
{
	Matrix<m, n, T> m1 = getLrGauss();

	T det = m1[0][0];

	for (int i = 1; i < n; i++)
		det *= m1[i][i];

	return det;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> Matrix<m, n, T>::getLrGauss() const
{
	Matrix<m, n, T> mat = *this;
	for (int i = 0; i < m; i++)
	{
		if (mat[i][i] == 0)
		{
			for (int j = i + 1; j < m; j++)
			{
				if (mat[j][i] != 0)
				{
					mat.swapRow(i, j);
					break;
				}
			}
		}
		for (int j = i + 1; j < m; j++)
		{
			if (mat[i][i] != 0)
			{
				mat.subRow(j, i, mat[j][i] / mat[i][i]);
			}
		}
	}
	return mat;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> Matrix<m, n, T>::getGaussEliMat() const
{
	Matrix<m, n, T> lr_mat = getLrGauss();

	for (int i = 0; i < m; i++)
	{
		lr_mat.divideRow(i, lr_mat[i][i]);
		for (int j = 0; j < i; j++)
		{
			lr_mat.subRow(j, i, lr_mat[j][i]);
		}
	}

	return lr_mat;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> Matrix<m, n, T>::getInverse() const
{
	Matrix<m, n, T> mat = *this;
	Matrix<m, n, T> inv = identity_matrix();

	for (int i = 0; i < m; i++)
	{
		if (mat[i][i] == 0)
		{
			for (int j = i + 1; j < m; j++)
			{
				if (mat[j][i] != 0)
				{
					mat.swapRow(i, j);
					inv.swapRow(i, j);
					break;
				}
			}
		}
		if (mat[i][i] == 0) // no inverse
		{
			inv.clearData();
			return inv;
		}

		T d = mat[i][i];
		mat.divideRow(i, d);
		inv.divideRow(i, d);
		for (int j = i + 1; j < m; j++)
		{
			T f = mat[j][i];
			mat.subRow(j, i, f);
			inv.subRow(j, i, f);
		}
	}

	for (int i = 1; i < m; i++)
	{
		for (int j = 0; j < i; j++)
		{
			T f = mat[j][i];
			mat.subRow(j, i, f);
			inv.subRow(j, i, f);
		}
	}

	return inv;

}

template<size_t m, size_t n, typename T>
Matrix<n, m, T> Matrix<m, n, T>::getTransposed() const
{
	Matrix<n, m, T> mat;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			mat[j][i] = data[i][j];
		}
	}
	return mat;
}

template<size_t m, size_t n, size_t o, typename T>
Matrix<m, o, T> operator* (const Matrix<m, n, T> &a, const Matrix<n, o, T> &b)
{
	Matrix<m, o, T> tmp;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < o; j++)
		{
			for (int k = 0; k < n; k++)
			{
				tmp[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return tmp;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> operator* (Matrix<m, n, T> a, const T &b)
{
	return a *= b;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> operator/ (Matrix<m, n, T> a, const T &b)
{
	return a /= b;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> operator+ (Matrix<m, n, T> a, const Matrix<m, n, T> &b)
{
	return a += b;
}

template<size_t m, size_t n, typename T>
Matrix<m, n, T> operator- (Matrix<m, n, T> a, const Matrix<m, n, T> &b)
{
	return a -= b;
}
