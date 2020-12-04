// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;
  

private:
    double numVals;
    double maxLoadFactor;
    double numBuckets;
    struct Node{
        Node(KeyType k, ValueType v){
            key = k;
            val = v;
            next = nullptr;
        }
        Node(){
            next = nullptr;
        }
        ~Node(){
 
        }
        KeyType key;
        ValueType val;
        Node* next = nullptr;
    };
    Node** heads;
    unsigned int getBucketNumber(const KeyType& key, int nbuck) const;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    maxLoadFactor = maximumLoadFactor;
    numBuckets = 8;
    numVals = 0;
    heads = new Node*[8];
    for (int a = 0; a < 8; a++){
        heads[a] = nullptr;
    }
   
}
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::~ExpandableHashMap()
{

    for (int a = 0; a < numBuckets; a++){
        Node* ptr = heads[a];
        while(ptr!=nullptr){
            Node* temp = ptr;
            ptr = ptr->next;
            delete temp;
        }
    }
    delete[] heads;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType,ValueType>::reset()
{
    for (int a = 0; a < numBuckets; a++){
        Node* ptr = heads[a];
        while(ptr!=nullptr){
            Node* temp = ptr;
            ptr = ptr->next;
            delete temp;
        }
    }
    delete[] heads;
    
    numBuckets = 8;
    numVals = 0;
    heads = new Node*[8];
    for (int a = 0; a < 8; a++){
        heads[a] = nullptr;
    }
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType,ValueType>::size() const
{
    return numVals;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType,ValueType>::associate(const KeyType& key, const ValueType& value)
{
    ValueType* val = find(key);
    if(val!=nullptr){
        *val = value;
        return;
    }
    double load = (numVals+1.0)/numBuckets;

    if (load > maxLoadFactor){
        int expandedSize = numBuckets*2;
        Node** expandedHashMap = new Node*[expandedSize];
        for (int i = 0; i < numBuckets*2; i++){
            expandedHashMap[i] = nullptr;
        }
        for (int a = 0; a < numBuckets; a++){
            Node* ptr = heads[a];
            while(ptr!=nullptr){
                int indexToPutAtInExpandedHMap = getBucketNumber(ptr->key,numBuckets*2); //get a bucket number based on twice as many buckets
                Node* ptrToLinkedListOfInterestInExpandedMap = expandedHashMap[indexToPutAtInExpandedHMap];
                if(ptrToLinkedListOfInterestInExpandedMap==nullptr){
                    expandedHashMap[indexToPutAtInExpandedHMap] = new Node(ptr->key,ptr->val);
                }
                else{
                    while(ptrToLinkedListOfInterestInExpandedMap->next!=nullptr){ //check
                        ptrToLinkedListOfInterestInExpandedMap = ptrToLinkedListOfInterestInExpandedMap->next;
                    }
                    ptrToLinkedListOfInterestInExpandedMap->next = new Node(ptr->key,ptr->val);
                }
                ptr = ptr->next;
            }
        }
    

        for (int a = 0; a < numBuckets; a++){
            Node* ptr = heads[a];
            while(ptr!=nullptr){
                Node* temp = ptr;
                ptr = ptr->next;
                delete temp;
            }
        }
        delete[] heads;
        
        heads = expandedHashMap;
        numBuckets = numBuckets * 2;
    }
    
   //associate new stuff
        int indexToPutAt = getBucketNumber(key,numBuckets);
        Node* ptrToLinkedListOfInterest = heads[indexToPutAt];
        if(ptrToLinkedListOfInterest==nullptr){
            heads[indexToPutAt] = new Node(key,value);
        }
        else{
            while(ptrToLinkedListOfInterest->next!=nullptr){
                ptrToLinkedListOfInterest = ptrToLinkedListOfInterest->next;
            }
            ptrToLinkedListOfInterest->next = new Node(key,value);
        }
    

    numVals++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType,ValueType>::find(const KeyType& key) const
{
    int indexItWouldBeAt = getBucketNumber(key,numBuckets);
    Node* ptrToLinkedListOfInterest = heads[indexItWouldBeAt];
    while(ptrToLinkedListOfInterest!=nullptr){
        if(ptrToLinkedListOfInterest->key==key){
            return &(ptrToLinkedListOfInterest->val);
        }
        ptrToLinkedListOfInterest = ptrToLinkedListOfInterest->next;
    }
    return nullptr;
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType,ValueType>::getBucketNumber(const KeyType& key, int nbuck) const{
    unsigned int hasher(const KeyType& k); //prototype
    unsigned int h = hasher(key);
    return (h%nbuck);
}

