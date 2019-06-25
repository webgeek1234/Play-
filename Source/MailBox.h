#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

class CMailBox
{
public:
	virtual ~CMailBox() = default;

	typedef std::function<void()> FunctionType;

	void SendCall(const FunctionType&, bool = false, bool = false);
	void SendCall(FunctionType&&);
	void FlushCalls();

	bool IsPending() const;
	bool ReceiveCall();
	void WaitForCall();
	void WaitForCall(unsigned int);
	void SetCanWait(bool);
	void ProcessUntilBreakPoint();
	void Reset();
	void Release();

private:
	struct MESSAGE
	{
		MESSAGE() = default;

		MESSAGE(MESSAGE&&) = default;
		MESSAGE(const MESSAGE&) = delete;

		MESSAGE& operator=(MESSAGE&&) = default;
		MESSAGE& operator=(const MESSAGE&) = delete;

		FunctionType function;
		int id;
		bool sync;
		bool breakpoint;
	};

	typedef std::deque<MESSAGE> FunctionCallQueue;

	FunctionCallQueue m_calls;
	std::mutex m_callMutex;
	std::condition_variable m_callFinished;
	std::condition_variable m_waitCondition;
	bool m_canWait = true;
	int m_count = 0;
	int m_processedID = 0;
};
