#include<iostream>
#include<mutex>
#include<vector>
#include<queue>
#include<functional>
#include<condition_variable>
#include<future>
#include<memory>
#include<thread>
using namespace std;


class ThreadPool {
public:
	ThreadPool(int n) :stop(false) {
		for (int i = 1; i <= n; i++) {
			workers.emplace_back([this, i] {
				while (true) {
					function<void()> task;
					{
						unique_lock<mutex> lock(mtx);
						cv.wait(lock, [this] {return !q.empty() || stop; });
						if (q.empty() && stop)return;
						task = move(q.front());
						q.pop();
					}
					task();
				}
				});
		}
	}

	template<class F,class ...Args>
	auto enqueue(F&& f, Args&&...args) -> future<typename result_of<F(Args...)>::type> {
		using return_type = typename result_of<F(Args...)>::type;

		auto task = make_shared < packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));

		future<return_type> res = task->get_future();
		{
			lock_guard<mutex> lock(mtx);
			if (stop)throw runtime_error("  ");
			q.emplace([task] {(*task)(); });
		}
		cv.notify_one();
		return res;
	}

	~ThreadPool() {
		{
			lock_guard<mutex> lock(mtx);
			stop = true;
		}
		cv.notify_all();
		for (thread& t : workers) {
			t.join();
		}
	}


private:
	bool stop;
	vector<thread> workers;
	queue < function<void()>> q;
	mutex mtx;
	condition_variable cv;
};



int main() {

	ThreadPool pool(4);

	auto res1 = pool.enqueue([](int a, int b,int c) {return a * b + c; }, 2, 3, 4);
	cout << res1.get() << endl;

	return 0;
}