#ifndef __ZOO_SIG_SLOT_H__
#define __ZOO_SIG_SLOT_H__

#include <zoo/Support.h>
#include <zoo/UserData.h>
#include <zoo/Log.h>

namespace zoo {

class _zooExport Signal
{
	friend class SignalTrigger;
	DISALLOW_COPY_AND_ASSIGN(Signal)
public:
	Signal();
	virtual ~Signal();
	UserData& userData() { return _userData; }
	const UserData& userData() const { return _userData; }
	PROPERTY_R(int, _sigid, SignalID)

private:
	UserData                    _userData;
	mutable vector<class Slot*> _slotList;
};

class Slot
{
public:
	virtual ~Slot() {}
	virtual bool operator()(const Signal& sig) = 0;
};

class SlotLambda : public Slot
{
	friend class SignalTrigger;
	SlotLambda(std::function<void()> lamb) : _lambda(lamb) {}
	bool operator()(const Signal& sig)
	{
		if (!_lambda)
			return false;
		_lambda();
		return true;
	}

private:
	std::function<void()> _lambda;
};

class SlotFunction : public Slot
{
	friend class SignalTrigger;
public:
	typedef void (*SLOT_FUNCTION)(const UserData& userdata);
	SlotFunction(SLOT_FUNCTION func) : _function(func) {}
	bool operator()(const Signal& sig)
	{
		if (!_function)
			return false;
		_function(sig.userData());
		return true;
	}

private:
	SLOT_FUNCTION _function;
};

template<typename T>
class SlotMethod : public Slot
{
	friend class SignalTrigger;
public:
	typedef void (T::*SLOT_METHOD)(const UserData& userdata);
	SlotMethod(T* caller, SLOT_METHOD meth) : _caller(caller), _method(meth) {}
	bool operator()(const Signal& sig)
	{
		if (!_caller || !_method)
			return false;
		(_caller->*(_method))(sig.userData());
		return true;
	}

private:
	T*          _caller;
	SLOT_METHOD _method;
};

class _zooExport SignalTrigger
{
public:
	static void trigger(const Signal& sig);
	static void connect(const Signal& sig, std::function<void()> lamb);
	static void connect(const Signal& sig, SlotFunction::SLOT_FUNCTION func);
	static void disconnect(const Signal& sig, SlotFunction::SLOT_FUNCTION func);
	template<typename T> static void connect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD meth);
	template<typename T> static void disconnect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD meth);
	static void disconnect(const Signal& sig);
	static bool hasSignal(const Signal& sig);
};

template<typename T>
void SignalTrigger::connect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD meth)
{
	ZOO_ASSERT(caller && meth);
	vector<Slot*>::const_iterator it = sig._slotList.cbegin();
	for (; it != sig._slotList.cend(); ++it)
	{
		SlotMethod<T>* item = dynamic_cast<SlotMethod<T>*>(*it);
		if (NULL != item && caller == item->_caller && meth == item->_method)
		{
			zoo_warning("The slot don't support same method in same signal id %d", sig.getSignalID());
			return;
		}
	}
	sig._slotList.push_back(new SlotMethod<T>(caller, meth));
}

template<typename T>
void SignalTrigger::disconnect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD meth)
{
	ZOO_ASSERT(caller && meth);
	vector<Slot*>::const_iterator it = sig._slotList.cbegin();
	for (; it != sig._slotList.cend(); ++it)
	{
		SlotMethod<T>* item = dynamic_cast<SlotMethod<T>*>(*it);
		if (NULL != item && caller == item->_caller && meth == item->_method)
		{
			SAFE_DELETE(item);
			sig._slotList.erase(it);
			break;
		}
	}
}

}

#endif // __ZOO_SIG_SLOT_H__
