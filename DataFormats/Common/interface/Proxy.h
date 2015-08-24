#ifndef DataFormats_Common_Proxy_h
#define DataFormats_Common_Proxy_h

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Provenance/interface/Provenance.h"
#include "DataFormats/Common/interface/TestHandle.h"

#include <iterator>

/* \class Association<RefProd>
 *
 * \ Johan Juntunen, HIP
 *
 *
 * provides data handler and iterator for AssociationToManyWithQuality
 *
 */

namespace edm {
  // T is vector
	template <typename U>
	class ProxyIterator	{
	public:
    typedef ProxyIterator<U> iterator;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename U::value_type value_type;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<iterator> const_reverse_iterator;

		ProxyIterator(U *values, int pos) : values_(values), currentPosition_(pos)	{	}

		value_type get(int index)	{	return values_->at(index);	}

		value_type &operator*() { return (*values_)[currentPosition_]; }

    value_type *operator->() { return &(operator*());  }

    int size()
		{
			return values_->size();
		}

    int getCurrentPosition()  { return currentPosition_;  }
	
    // pre-increment
		iterator &operator++()
    {
      ++currentPosition_;
		  return *this;
	  }

    //post-increment
		iterator operator++(int)
    {
      iterator temp(*this);
      ++(*this);
      
		  return temp;
	  }

    // pre-decrement
	  iterator &operator--()
	  {
      if(currentPosition_ != 0)
		    currentPosition_--;
		  return *this;
	  }

    // post-decrement
	  iterator operator--(int)
	  {
      iterator temp(*this);
      --(*this);
		  return *this;
	  }

    bool operator== (ProxyIterator<U> other) const
    {
		  //compare first valuevectors
      if( values_ == other.values_ )
      {
        // if valuevectors are the same, compare the positions
        if( currentPosition_ == other.getCurrentPosition() )
          return true;
        else
          return false;
      }
      else
        return false;
    }

    int index2 = 0;
		bool operator!= (ProxyIterator<U> other) const
	  {
      //compare first valuevectors
      if( values_ != other.values_ )
        return true;
      // if valuevectors are the same, compare the positions
      else
      {
        if( currentPosition_ != other.getCurrentPosition() )
          return true;
        else
          return false;
      }
	  }

  private:
		U *values_;
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
		
		ProxyIterator<T> begin()	{	return ProxyIterator<T>(&values_, 0); }

		ProxyIterator<T> end()	{	return ProxyIterator<T>(&values_, size()); }

    std::vector<Ref<T>> getValues()
    {
      std::vector<Ref<T>> temp;
      
      //TODO: add storing of pidK upon insertion of new values, so that
      //      the dummy pidK below can be replaced with that one
      ProductID const pidK(-1, -1);

      for(unsigned int i = 0; i < ids_.size(); i++)
        temp.push_back(Ref<T>(TestHandle<T>(&values_,pidK),i));

      return temp;
    }




	private:
    std::vector<int> ids_;
		T values_;

		

	};

}

#endif
