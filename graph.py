from matplotlib import pyplot as plt

def plot_data(d1, d2, d3):
	xvals = [20, 50, 100, 500, 1000, 2000, 3000, 4000]
	plt.title("Comparison of Sequential vs 4-thread OMP vs 4-Thread PThreads")
	plt.xlabel("Square Matrix Size")
	plt.ylabel("Run Time (seconds)")
	plt.plot(xvals, d1[:8], 'b-', label='Sequential')
	plt.plot(xvals, d1[8:16], 'r-', label='OMP')
	plt.plot(xvals, d1[16:], 'g-', label='PThreads')
	plt.legend()

	plt.savefig('full_comp.png', bbox_inches='tight')
	plt.clf()

	plt.title("Comparison of OMP with Various Thread Counts")
	plt.xlabel("Square Matrix Size")
	plt.ylabel("Run Time (seconds)")
	plt.plot(xvals, d2[:8], 'b-', label='2')
	plt.plot(xvals, d2[8:16], 'r-', label='4')
	plt.plot(xvals, d2[16:24], 'g-', label='6')
	plt.plot(xvals, d2[24:], 'c-', label='8')
	plt.legend()

	plt.savefig('omp_comp.png', bbox_inches='tight')
	plt.clf()

	plt.title("Comparison of PThreads with Various Thread Counts")
	plt.xlabel("Square Matrix Size")
	plt.ylabel("Run Time (seconds)")
	plt.plot(xvals, d3[:8], 'b-', label='2')
	plt.plot(xvals, d3[8:16], 'r-', label='4')
	plt.plot(xvals, d3[16:24], 'g-', label='6')
	plt.plot(xvals, d3[24:], 'c-', label='8')
	plt.legend()

	plt.savefig('pt_comp.png', bbox_inches='tight')
	plt.clf()

	plt.title("Comparison of OMP vs PThreads with 8 Threads")
	plt.xlabel("Square Matrix Size")
	plt.ylabel("Run Time (seconds)")
	plt.plot(xvals, d2[24:], 'r-', label='OMP')
	plt.plot(xvals, d3[24:], 'b-', label='Pthreads')
	plt.legend()

	plt.savefig('omp_pt_comp.png', bbox_inches='tight')

def load_data(fn):
	data = []
	with open(fn) as f:
		for l in f.readlines():
			data.append(float(l.strip('\n')))
	return data

def main():
	data1 = load_data('full_comp.txt')
	data2 = load_data('omp_vals.txt')
	data3 = load_data('pt_vals.txt')

	plot_data(data1, data2, data3)

if __name__ == '__main__':
	main()