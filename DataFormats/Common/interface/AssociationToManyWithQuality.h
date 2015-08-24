#ifndef DataFormats_Common_AssociationToManyWithQuality_h
#define DataFormats_Common_AssociationToManyWithQuality_h
/* \class AssociationToManyWithQuality<RefProd>
 *
 * \ Johan Juntunen, HIP
 *
 *
 * \copied and modified from Association.h and ValueMap.h made by Luca Lista, INFN
 *
 */

#include "DataFormats/Common/interface/CMS_CLASS_VERSION.h"
#include "DataFormats/Provenance/interface/ProductID.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include <map>

#include "DataFormats/Common/interface/Proxy.h"

// first implementation without the quality value Q
namespace edm {
  // CVal is for example std::vector<double>
  template<typename CVal>
  class AssociationToManyWithQuality {
  public:

    AssociationToManyWithQuality() { }
    template<typename H>
    explicit AssociationToManyWithQuality(const H & h) : values_(*h) {}


 
    //template<typename RefKey>
    template <typename T>
    Proxy<CVal> operator[](const Ref<T> & r) const {

			auto begin = std::lower_bound(ids_.begin(),ids_.end(), r.key());

			auto end = std::upper_bound(ids_.begin(),ids_.end(), r.key());
    
			return Proxy<CVal>((int)std::distance<std::vector<int>::const_iterator>(ids_.begin(),begin) , (int)std::distance<std::vector<int>::const_iterator>(ids_.begin(),end), ids_, associatedValues_);
    }

	
		// Parameters:
		//						h : (std::vector<int>) vector of keyvalues, that has associations to items in ref_
		//						begin, end : begin and end iterators to multimap that describes the associations
		template<typename H>
		void insert(const H & h, std::multimap<int,int> multimap) {

			// check first that there are ids
			if(h->size() < 1) throwIdError();

      // insert multimap values
      for(auto m : multimap)
      {
        int idIndex = m.first;
        int valIndex = m.second;
        // use -- operator, because map indexing starts from 1
        --idIndex;
        --valIndex;
			  // add id-value - pair to same positions in their vectors
        std::vector<int>::iterator upperIter = std::upper_bound(ids_.begin(), ids_.end(), (*h)[idIndex]);
        ids_.insert(ids_.begin()+(upperIter-ids_.begin()), (*h)[idIndex]);

        // ids_ - vector has changed, so upperIter is invalidated => update the reference
        upperIter = std::upper_bound(ids_.begin(), ids_.end(), (*h)[idIndex]);

        if( (int)associatedValues_.size() < (int)(upperIter-ids_.begin()) )
          associatedValues_.push_back(values_[valIndex]);
        else
          associatedValues_.insert(associatedValues_.begin() + (int)(upperIter-ids_.begin()), values_[valIndex]);

      }
	
		}


		edm::Ref<CVal> get(int id) const { 
			
			// find matching id position
			for(int i = 0; i < ids_.size(); i++)	{
				
				if( ids_[i] == id)	{
          // use position i as key value in the constructor
					return edm::Ref<CVal>(associatedValues_[i], id);
				}
			}

			
			return edm::RefVector<CVal>();
    }

    AssociationToManyWithQuality<CVal> & operator+=(const AssociationToManyWithQuality<CVal> & o) {
      add(o);
      return *this;
    }

		// returns true if key object with the given id exists
    bool contains(int id) const { 
			for(int i = 0; i < size(); i++)	{
				if(ids_.at(i) == id)
					return true;
			}

			return false; 
		}

    int size() const { return ids_.size(); }
    bool empty() const { return ids_.empty(); }
    void clear() { 
			//ref_ = edm::RefProd<associated_type>();
			ids_.clear();
			associatedValues_.clear();

		}


    //refprod_type ref() const { return ref_; }
 
    void swap(AssociationToManyWithQuality& other) {
			ids_.swap(other.ids_);
			associatedValues_.swap(other.associatedValues_);
      //this->ValueMap<int>::swap(other);
      //ref_.swap(other.ref_);
    }
    AssociationToManyWithQuality& operator=(AssociationToManyWithQuality const& rhs) {
      AssociationToManyWithQuality temp(rhs);
      this->swap(temp);
      return *this;
    }

  
    //Used by ROOT storage
    CMS_CLASS_VERSION(10)

  private:
    // ids_ sorted by the order of magnitude
		std::vector<int> ids_;
    // unsorted values that are stored upon instantation of AssociationToManyWithQuality- object
    CVal values_;
		// contains all associated values, position matches the id position in the ids_ -vector
		CVal associatedValues_;


    void throwIdError() const {
      Exception::throwThis(errors::InvalidReference, "Association: there are no key values defined\n");
    }

    void throwRefSet() const {
      Exception::throwThis(errors::InvalidReference, "Association: reference to product already set\n");
    }

  }; 
  
  // Free swap function
  template <typename CVal>
  inline void swap(AssociationToManyWithQuality<CVal>& lhs, AssociationToManyWithQuality<CVal>& rhs) {
    std::vector<int> tempIds = lhs.ids_;
    CVal tempValues = lhs.values_;
		CVal tempAssociatedValues = lhs.associatedValues_;
    lhs.ids_ = rhs.ids_;
    lhs.values_ = rhs.values_;
    lhs.associatedValues_ = rhs.associatedValues_;
    rhs.ids_ = tempIds;
    rhs.values_ = tempValues;
    rhs.associatedValues_ = tempAssociatedValues;
  }

  template<typename CVal>
  inline AssociationToManyWithQuality<CVal> operator+( const AssociationToManyWithQuality<CVal> & a1,
				       const AssociationToManyWithQuality<CVal> & a2 ) {
    // concatenate the member vectors
    AssociationToManyWithQuality<CVal> a = a1;
    a.ids_.insert(a.ids_.end(), a2.ids_);
    a.values_.insert(a.values_.end(), a2.values_);
    a.associatedValues_.insert(a.associatedValues_.end(), a2.associatedValues_);
    
    return a;
  }
}

#endif
