#ifndef DataFormats_Common_Proxy_h
#define DataFormats_Common_Proxy_h

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Common/interface/TestHandle.h"

/* \class Association<RefProd>
 *
 * \ Johan Juntunen, HIP
 *
 *
 * \copied and modified from Association.h and ValueMap.h made by Luca Lista, INFN
 *
 */

namespace edm {

	template <typename T>
	class ProxyIterator	{
	public:
		ProxyIterator(T values, int pos) : values_(values), currentPosition_(pos)	{	}

		T get(int index)	{	return values_->at(index);	}

		T const& operator*() const
    {
       return values_[currentPosition_];
    }

    int size()
		{
			return values_.size();
		}
	
		ProxyIterator<T> const& operator++ ()
    {
      if(currentPosition_ != (size()-1))
        currentPosition_++;
		  return *this;
	  }

	  ProxyIterator<T> const& operator-- ()
	  {
      if(currentPosition_ != 0)
		    currentPosition_--;
		  return *this;
	  }


		bool operator!= (const ProxyIterator<T>& other) const
	  {
		  return (values_[currentPosition_]) == *(other.get(currentPosition_));
	  }

  private:
		const T values_;
		int currentPosition_;

	};
  
  // T is vector
  template<typename T>
  class Proxy {
  public:
		
		// parameters :
		//				startIndex, endIndex : indexes that limit the range of elements to be collected
		//				values : vector of which the subset defined by the indexes is taken
		Proxy(int startIndex, int endIndex, std::vector<int> ids, T values)
		{
			typename std::vector<int>::iterator firstId = ids.begin() + startIndex;
			typename std::vector<int>::iterator lastId = ids.begin() + endIndex;

			ids_ = std::vector<int>(firstId, lastId);

			typename T::iterator firstVal = values.begin() + startIndex;
			typename T::iterator lastVal = values.begin() + endIndex;

			values_ = T(firstVal, lastVal);
		}

		int size()
		{
			return values_.size();
		}
		
		ProxyIterator<T> begin()	{	return ProxyIterator<T>(values_, 0); }

		ProxyIterator<T> end()	{	return ProxyIterator<T>(values_, size()); }

    std::vector<Ref<T>> getValues()
    {
      std::vector<Ref<T>> temp;
      //for(typename T::value_type it : values_)
      
      //TODO: add storing of pidK upon insertion of new values, so that
      //      the dummy pidK below can be replaced with that one
      ProductID const pidK(-1, -1);

      for(unsigned int i = 0; i < ids_.size(); i++)
        temp.push_back(Ref<T>(TestHandle<T>(&values_,pidK),i));//&values_[i], pidK), ids_[i]));

      //edm::TestHandle<T> handle = edm::TestHandle<T>(&values_, pidK);
      //temp = Ref<T>(handle);
      return temp;
    }




	private:
    std::vector<int> ids_;
		T values_;

		

	};

}

#endif
