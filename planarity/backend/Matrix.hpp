#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <cassert>
#include <stdexcept>

template < typename T = double > class Matrix {
      private:
	std::vector < T * >rows_;
	std::vector < T > data_;
	int m_, n_;
	void Copy(const Matrix & src) {
		memcpy(&data_[0], &src.data_[0], sizeof(T) * n_ * m_);
	} void Init(int m, int n) {
		n_ = n;
		m_ = m;
		rows_.resize(m);
		data_.resize(n * m);
		for (int i = 0; i < m; ++i)
			rows_[i] = &data_[i * n];
	}
#define EPSILON (1E-9)
#define MATRIX_CHECK_QUADRIC(mtr) assert((mtr).n_ == (mtr).m_)
      public:
	void Print(FILE * f) {
		for (int i = 0; i < m_; ++i, fprintf(f, "\n"))
			for (int j = 0; j < n_; ++j)
				fprintf(f, "%5.2f ", (float) rows_[i][j]);
	}
	int n() const {
		return n_;
	} int m() const {
		return m_;
	} Matrix()
	:m_(0), n_(0) {
	}
	Matrix(int m, int n) {
		Init(m, n);
	}
	Matrix(const Matrix & src) {
		Init(src.m_, src.n_);
		Copy(src);
	}
	Matrix & operator=(const Matrix & src) {
		if (src.m_ != m_ || src.n_ != n_)
			Init(src.m_, src.n_);
		Copy(src);
	}
	void SetZero() {
		memset(&data_[0], 0, sizeof(T) * n_ * m_);
	}
	void SetIdentity() {
		MATRIX_CHECK_QUADRIC(*this);
		SetZero();
		for (int i = n_ - 1; i >= 0; --i)
			rows_[i][i] = 1;
	}
	T *operator[] (int i) {
		return rows_[i];
	}
      private:
	void SwapAndDivideRow(int a, int b, T divisor) {
		T *tmp = rows_[a];
		rows_[a] = rows_[b];
		rows_[b] = tmp;
		T *row = rows_[b];
		for (int j = n_ - 1; j >= 0; --j)
			row[j] /= divisor;
	}
	void SubtractMultipliedRow(int from, int what, T mult) {
		T *a = rows_[from], *b = rows_[what];
		for (int j = n_ - 1; j >= 0; --j)
			a[j] -= mult * b[j];
	}
      public:
	void SetInverseOf(const Matrix & src) {
		MATRIX_CHECK_QUADRIC(src);
		Matrix tmpM(src);
		if (m_ != src.m_ || n_ != src.m_)
			Init(src.m_, src.m_);
		SetIdentity();
		int n = n_;
		for (int icol = 0; icol < n; icol++) {
			T maxv = -1, d;
			int maxi = -1, i;
			for (int i = icol; i < n; i++) {
				d = fabs(tmpM[i][icol]);
				if (d > maxv) {
					maxv = d;
					maxi = i;
				}
			}
			if (maxv < EPSILON)
				throw std::
				    runtime_error("Noninvertible matrix");

			d = tmpM[maxi][icol];
			tmpM.SwapAndDivideRow(maxi, icol, d);
			SwapAndDivideRow(maxi, icol, d);
			for (int i = 0; i < n; i++)
				if (i != icol) {
					d = tmpM[i][icol];
					tmpM.SubtractMultipliedRow(i, icol,
								   d);
					SubtractMultipliedRow(i, icol, d);
				}
		}
	}
	void SetProduct(const Matrix & l, const Matrix & r) {
		assert(l.n_ == r.m_);
		if (m_ != l.m_ || n_ != r.n_)
			Init(l.m_, r.n_);
		const std::vector < T * >&lrows = l.rows_, &rrows =
		    r.rows_;
		std::vector < T * >&prows = rows_;
		int ni = l.m_, nj = r.n_, nk = l.n_;
		for (int i = 0; i < ni; ++i)
			for (int j = 0; j < nj; ++j) {
				T t = 0;
				for (int k = 0; k < nk; ++k)
					t += lrows[i][k] * rrows[k][j];
				prows[i][j] = t;
			}
	}
};

#undef EPSILON
#undef MATRIX_CHECK_QUADRIC

#endif				//MATRIX_H
