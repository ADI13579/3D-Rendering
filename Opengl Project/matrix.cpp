// C++ program to find adjoint and inverse of a matrix
#include<iostream>
using namespace std;
#define N 4

// Function to get cofactor of A[p][q] in temp[][]. n is current
// dimension of A[][]
void getCofactor(float A[N][N], float temp[N][N], int p, int q, int n)
{
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			// Copying into temporary matrix only those element
			// which are not in given row and column
			if (row != p && col != q)
			{
				temp[i][j++] = A[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
}

/* Recursive function for finding determinant of matrix.
n is current dimension of A[][]. */
int determinant(float A[N][N], int n)
{
	int D = 0; // Initialize result

	// Base case : if matrix contains single element
	if (n == 1)
		return A[0][0];

	float temp[N][N]; // To store cofactors

	int sign = 1; // To store sign multiplier

	// Iterate for each element of first row
	for (int f = 0; f < n; f++)
	{
		// Getting Cofactor of A[0][f]
		getCofactor(A, temp, 0, f, n);
		D += sign * A[0][f] * determinant(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return D;
}

// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(float A[N][N], float adj[N][N])
{
	if (N == 1)
	{
		adj[0][0] = 1;
		return;
	}

	// temp is used to store cofactors of A[][]
	int sign = 1;
	float temp[N][N];

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// Get cofactor of A[i][j]
			getCofactor(A, temp, i, j, N);

			// sign of adj[j][i] positive if sum of row
			// and column indexes is even.
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			// Interchanging rows and columns to get the
			// transpose of the cofactor matrix
			adj[j][i] = (sign) * (determinant(temp, N - 1));
		}
	}
}

// Function to calculate and store inverse, returns false if
// matrix is singular
bool inverse(float A[N][N], float inverse[N][N])
{
	// Find determinant of A[][]
	int det = determinant(A, N);
	if (det == 0)
	{
		std::cout << "Singular matrix, can't find its inverse";
		return false;
	}

	// Find adjoint
	float adj[N][N];
	adjoint(A, adj);

	// Find Inverse using formula "inverse(A) = adj(A)/det(A)"
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			inverse[i][j] = adj[i][j] / float(det);

	return true;
}

// Generic function to display the matrix. We use it to display
// both adjoin and inverse. adjoin is integer matrix and inverse
// is a float.
template<class T>
void display(T A[N][N])
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << A[i][j] << " ";
		cout << endl;
	}
}


// C++ Program to find
// This function stores transpose of A[][] in B[][]
void transpose(float A[][N], float B[][N])
{
	int i, j;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			B[i][j] = A[j][i];
}


/*
// Driver code
int main()
{
	int A[N][N] = { {1, 1, 1, 1},
					{2, 2, 2, 2},
					{3, 3, 3, 3},
					{4, 4, 4, 4} };

	int B[N][N], i, j;

	transpose(A, B);

	cout << "Result matrix is \n";
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			cout << " " << B[i][j];
		cout << "\n";
	}

	return 0;
}
*/




// Driver program/*
/*
int main()
{
	float A[N][N] = { {5, -2, 2, 7},
					{1, 0, 0, 3},
					{-3, 1, 5, 0},
					{3, -1, -9, 4} };

	float adj[N][N]; // To store adjoint of A[][]

	float inv[N][N]; // To store inverse of A[][]

	cout << "Input matrix is :\n";
	display(A);

	cout << "\nThe Adjoint is :\n";
	adjoint(A, adj);
	display(adj);

	cout << "\nThe Inverse is :\n";
	if (inverse(A, inv))
		display(inv);

	return 0;
}
/*/