#include <queue>
#include <iostream>
using namespace std;

bool func(int a, int b) {
	return a < b;
}
int main()
{
	priority_queue<int, std::vector<int>, decltype(&func)> q(&func);
	for (int i = 0; i < 10; i++) { q.push(i); }
	while (!q.empty()) {
		cout << q.top() << endl;
		q.pop();
	}
	return 0;
}