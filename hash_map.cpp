#include<vector>
#include<string>
#include<list>
#include<initializer_list>
#include<iterator>
#include <iostream>
#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <map>

template < class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
private:
	unsigned static const defaultCapacity=8;
	unsigned static const rehash_size_more = 2;
	unsigned static const rehash_size_less = 4;
	unsigned capacity;
	unsigned count;
	std::hash<KeyType> hasher;
	typename std::vector<std::list<std::pair<const KeyType, ValueType> > > table;
	
public:
	class iterator {
	public:
		HashMap<KeyType, ValueType, std::hash<KeyType> > *link;
		typename std::vector<std::list<std::pair<const KeyType, ValueType> > >::iterator on_list;
		typename std::list<std::pair<const KeyType, ValueType> >::iterator on_pair;

		iterator() {

		}
		iterator(HashMap<KeyType, ValueType, std::hash<KeyType> > *link_copy,
			typename std::vector<std::list<std::pair<const KeyType, ValueType> > >::iterator on_list_copy,
			typename std::list<std::pair<const KeyType, ValueType> >::iterator on_pair_copy) {
			this->link = link_copy;
			this->on_list = on_list_copy;
			this->on_pair = on_pair_copy;
		}
		iterator(const iterator &temp) {
			this->link = temp.link;
			this->on_list = temp.on_list;
			this->on_pair = temp.on_pair;
		}

		bool operator ==(iterator temp) {
			return (this->on_list == temp.on_list) && (this->on_pair == temp.on_pair);
		}
		bool operator !=(iterator temp) {
			return !(*this == temp);
		}

		std::pair<const KeyType, ValueType> operator*() {
			return *(this->on_pair);
		}
		typename std::list<std::pair<const KeyType, ValueType> >::iterator operator ->() {
			return this->on_pair;
		}
		std::pair<const KeyType, ValueType> operator &() {
			return *this->on_pair;
		}

		iterator operator++() {
			++this->on_pair;
			if (this->on_pair == this->on_list->end()) {
				return *this;
			} else {
				auto temp1 = this->on_list;
				while (temp1 != link->table.end()) {
					auto r = temp1;
					++temp1;
					if (temp1 == link->table.end()) {
						this->on_list = r;
						this->on_pair = this->on_list->end();
						return *this;
					}
					if (temp1->size() != 0) {
						this->on_list = temp1;
						this->on_pair= this->on_list->begin();
						return *this;
					}
				}
			}
			return *this;
		}
		iterator operator++(int) {
			iterator pred = *this;
			++this;
			return pred;
		}
	};
public:
	class const_iterator {
	public:
		HashMap<KeyType, ValueType, std::hash<KeyType> > *link;
		typename std::vector<std::list<const std::pair<const KeyType, ValueType> > >::iterator on_list;
		typename std::list<const std::pair<const KeyType, ValueType> >::iterator on_pair;
		const_iterator() {

		}
		const_iterator(HashMap<KeyType, ValueType, std::hash<KeyType> > *link_copy,
			typename std::vector<std::list<const std::pair<const KeyType, ValueType> > >::iterator on_list_copy,
			typename std::list<const std::pair<const KeyType, ValueType> >::iterator on_pair_copy) {
			this->link = link_copy;
			this->on_list = on_list_copy;
			this->on_pair = on_pair_copy;
		}
		const_iterator(const const_iterator &temp) {
			this->link = temp.link;
			this->on_list = temp.on_list;
			this->on_pair = temp.on_pair;
		}
		bool operator ==(const_iterator temp) const {
			return this->on_list = temp.on_list && this->on_pair = temp.on_pair;
		}
		const_iterator operator =(const_iterator temp) {
			this->link = temp.link;
			this->on_list = temp.on_list;
			this->on_pair = temp.on_pair;
		}
		bool operator !=(const_iterator temp) const {
			return !(*this == temp);
		}
		typename std::list<const std::pair<const KeyType, ValueType> >::const_iterator operator ->() const {
			return this->on_pair;
		}
		const std::pair<const KeyType, ValueType> operator &() const {
			return *this->on_pair;
		}
		const std::pair<const KeyType, ValueType> operator*() {
			return this->on_pair;
		}
		const_iterator operator++() {
			++this->on_pair;

			if (this->on_pair != this->on_list->end()) {
				return *this;
			} else {
				auto temp1 = this->on_list;
				while (temp1 != link->table.end()) {
					auto r = temp1;
					++temp1;
					if (temp1 == link->table.end()) {
						this->on_list = r;
						this->on_pair = this->on_list->end();
						return *this;
					}
					if (temp1->size() != 0) {
						this->on_list = temp1;
						this->on_pair = this->on_list->begin();
						return *this;
					}
				}
			}
		}
		const_iterator operator++(int) {
			const_iterator pred = *this;
			++this;
			return pred;
		}
	};

public:
	iterator begin() {
		if (count == 0) {
			return end();
		}
		for (size_t i = 0; i < table.size(); ++i) {
			if (table[i].size() > 0) {
				iterator ans(this, table.begin() + i, table[i].begin());
				return ans;
			}
		}
		return end();
	}
	const_iterator begin() const {
		if (count == 0) {
			return end();
		}
		for (size_t i = 0; i < table.size(); ++i) {
			if (table[i].size() > 0) {
				const_iterator ans(this, table.begin() + i, table[i].begin());
				return ans;
			}
		}
	}
	iterator end() {
		iterator temp(this, table.begin() + (table.size() - 1), table[table.size() - 1].end());
		return temp;
	}
	const_iterator end() const {
		const_iterator temp(this, table.begin() + (table.size() - 1), table[table.size() - 1].end());
		return temp;
	}

	HashMap(Hash user_hasher = std::hash<KeyType>()) {
        capacity = defaultCapacity;
        count = 0;
        hasher = user_hasher;
        table.clear();	
        table.resize(capacity);
	}
	HashMap(iterator begin_copy, iterator end_copy, Hash user_hasher = std::hash<KeyType>()) {
		capacity = defaultCapacity;
		count = 0;
		hasher = user_hasher;
		table.clear();
		table.resize(capacity);
		for (auto t = begin_copy; t != end_copy; ++t) {
			insert(*t);
		}
	}
	HashMap(std::initializer_list<std::pair<KeyType, ValueType> > pos_copy, Hash user_hasher = std::hash<KeyType>()) {
		capacity = defaultCapacity;
		count = 0;
		hasher = user_hasher;
		table.clear();
		table.resize(capacity);
		for (auto t : pos_copy) {
			insert(t);
		}
	}
	HashMap(std::pair<KeyType, ValueType> *begin, std::pair<KeyType, ValueType> *end, Hash user_hasher = std::hash<KeyType>()) {
		capacity = defaultCapacity;
		count = 0;
		hasher = user_hasher;
		table.clear();
		table.resize(capacity);
		for (auto t = begin; t != end; ++t) {
			insert(*t);
		}
	}
	
	HashMap operator= (HashMap copm) {
		this->clear();
		for (auto & lst : table) {
			for (auto & para : lst) {
				insert(para);
			}
		}
	}
	
	const Hash hash_function() const {
		return hasher;
	}
	
	iterator find(KeyType key) {
		int pos = hasher(key) % capacity;
		for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
			if (t->first == key) {
				iterator ans(this, table.begin() + pos, t);
				return ans;
			}
		}
		iterator ans(this, table.begin() + (table.size() - 1), table[table.size() - 1].end());
		return ans;
	}
	const_iterator find(KeyType key) const {
		int pos = hasher(key) % capacity;
		const_iterator ans;
		ans.link = *this;
		for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
			if (t->first == key) {
				ans.on_list = *table[pos];
				ans.on_pair = t;
				return ans;
			}
		}
		ans.on_list = *table[table.size() - 1];
		ans.on_pair = ans.on_list->end();
		return ans;
	}
	size_t size() const noexcept {
		return count;
	}
	bool empty() const noexcept {
		return (count == 0);
	}
	void resize() {
		if (count * rehash_size_more > capacity) {
			capacity *= 2;
		} else if (count * rehash_size_less < capacity) {
			capacity /= 2;
		} else {
			return ;
		}
		std::vector<std::list<std::pair<const KeyType, ValueType> > > newtable(capacity);
		for (auto & lst : table) {
			for (auto & para : lst) {
				int pos = hasher(para.first) % capacity;
				newtable[pos].push_back(para);
			}
		}
		table.clear();
		swap(newtable, table);
	}
	void insert(const std::pair<const KeyType, ValueType> &value) {
		int pos = hasher(value.first) % capacity;
		for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
			if (t->first == value.first) {
			    return ;
			}
		}
		++count;
		table[pos].push_back(value);
		resize();
	}
	void insert(std::pair<const KeyType, ValueType> && value) {
	    int pos = hasher(value.first) % capacity;
		for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
			if (t->first == value.first) {
			    return ;
			}
		}
		++count;
		table[pos].push_back(value);
		resize();
	}
	void erase(const KeyType &value) {
	    int pos = hasher(value) % capacity;
	    for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
	        if (t->first == value) {
	            --count;
	            table[pos].erase(t);
	            break;
	        }
	    }
		resize();
	}
	void erase(KeyType && value) {
	    int pos = hasher(value) % capacity;
	    for (auto t = table[pos].begin(); t != table[pos].end(); ++t) {
	        if (t->first == value) {
	            --count;
	            table[pos].erase(t);
	            break;
	        }
	    }
		resize();
	}
	
	ValueType &operator[](const KeyType &K) {
        auto to = find(K);
        if (to == end()) {
            insert({K, ValueType()});
        }
        to = find(K);
        return to->second;
    }

	const ValueType& at(const KeyType& value) {
	    auto t = find(value);
	    if (t == end()) {
	    	throw std::out_of_range("_Map_base::at");
	    } else {
	        const ValueType & temp = t->second;
	        return temp;
	    }
	}
	void clear() {
		capacity = 8;
		count = 0;
		for (auto & lst : table) {
			lst.clear();
		}
		table.resize(capacity);
	}
};

