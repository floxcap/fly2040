#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template <class T>
class Queue {
public:
	std::queue<T> mQueue;
	typename std::queue<T>::size_type mMaxQueueSize;

	std::mutex mMutex;
	std::condition_variable mFull;
	std::condition_variable mEmpty;

	std::atomic_bool mQuit{false};
	std::atomic_bool mFinished{false};

	Queue(const size_t size_max);

	bool push(T &&data);
	bool pop(T &data);
	bool popWait(T &data);
    bool popNoWait(T &data, bool &gotFrame);

    void notifyFullWaiting();
    void notifyEmptyWaiting();

    void flush();
    void reset();

    void lock();
    void unlock();

	// The queue has finished accepting input
	void finished();
	void quit();
};

template <class T>
Queue<T>::Queue(size_t size_max) :
		mMaxQueueSize{size_max} {
}

template <class T>
void Queue<T>::notifyFullWaiting()
{
    mFull.notify_all();
}

template <class T>
void Queue<T>::notifyEmptyWaiting()
{
    mEmpty.notify_all();
}

template <class T>
bool Queue<T>::push(T &&data)
{
	std::unique_lock<std::mutex> lock(mMutex);

	while (!mQuit && !mFinished)
	{
		if ((0 == mMaxQueueSize) || (mQueue.size() <= mMaxQueueSize))
		{
			mQueue.push(std::move(data));

			mEmpty.notify_all();
			return true;
		}
		else
		{
			mFull.wait(lock);
		}
	}

	return false;
}

template <class T>
bool Queue<T>::pop(T &data)
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mQuit)
	{
		if (!mQueue.empty())
		{
			data = std::move(mQueue.front());
			mQueue.pop();

			mFull.notify_all();
			return true;
		}
		else if (mQueue.empty() && mFinished)
		{
			return false;
		}
		else
		{
			mEmpty.wait(lock);
		}
	}

	return false;
}

template <class T>
bool Queue<T>::popWait(T &data)
{
	std::unique_lock<std::mutex> lock(mMutex);

	while (!mQuit)
	{
		if (!mQueue.empty())
		{
			data = std::move(mQueue.front());
			mQueue.pop();

			mFull.notify_all();
			return true;
		}
		else if (mQueue.empty() && mFinished)
		{
			return false;
		}
		else
		{
			mEmpty.wait(lock);
		}
	}

	return false;
}

template <class T>
bool Queue<T>::popNoWait(T &data, bool &gotFrame)
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mQuit)
	{
		if (!mQueue.empty())
		{
			data = std::move(mQueue.front());
			mQueue.pop();

			mFull.notify_all();
			gotFrame = true;
			return true;
		}
		else if (mQueue.empty() && mFinished)
		{
		    gotFrame = false;
			return false;
		}
		else
		{
		    gotFrame = false;
			return false;
		}
	}

	return false;
}

template <class T>
void Queue<T>::finished()
{
	mFinished.store(true);
	mEmpty.notify_all();
}

template <class T>
void Queue<T>::quit()
{
	mQuit.store(true);
	mEmpty.notify_all();
	mFull.notify_all();
}

template <class T>
void Queue<T>::flush()
{
    mQuit.store(true);
    mEmpty.notify_all();
    mFull.notify_all();

    std::queue<T> empty;
    std::swap(mQueue, empty);

    reset();
}

template <class T>
void Queue<T>::reset()
{
    mFinished.store(false);
    mQuit.store(false);
	std::queue<T> empty;
	std::swap(mQueue, empty);
    mEmpty.notify_all();
    mFull.notify_all();
}

template <class T>
void Queue<T>::lock()
{
    mQuit.store(true);
    mEmpty.notify_all();
    mFull.notify_all();

    mMutex.lock();
}

template <class T>
void Queue<T>::unlock()
{
    mMutex.unlock();
    mEmpty.notify_all();
    mFull.notify_all();
}
