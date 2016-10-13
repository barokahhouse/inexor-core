#pragma once

#include "inexor/macros/define_null_macro.hpp"
#include "inexor/macros/loop_macros.hpp"
#include "inexor/deprecated/databuf_template.hpp"
#include "inexor/deprecated/isclass.hpp"
#include <assert.h>

// no namespace needed

/// 
template <class T, int MINSIZE = 8> struct vector
{

    T *buf;
    int alen;
	int ulen;

	vector() : buf(NULL), alen(0), ulen(0)
    {
    }

    vector(const vector &v) : buf(NULL), alen(0), ulen(0)
    {
        *this = v;
    }

    ~vector() 
	{
		shrink(0);
		if(buf) free(buf);
	}

    vector<T> &operator=(const vector<T> &v)
    {
        shrink(0);
        if(v.length() > alen) growbuf(v.length());
        loopv(v) add(v[i]);
        return *this;
    }

    T &add(const T &x)
    {
        if(ulen==alen) growbuf(ulen+1);
        new (&buf[ulen]) T(x);
        return buf[ulen++];
    }

    T &add()
    {
        if(ulen==alen) growbuf(ulen+1);
        new (&buf[ulen]) T;
        return buf[ulen++];
    }

    T &dup()
    {
        if(ulen==alen) growbuf(ulen+1);
        new (&buf[ulen]) T(buf[ulen-1]);
        return buf[ulen++];
    }

    void move(vector<T> &v)
    {
        if(!ulen)
        {
            swap(buf, v.buf);
            swap(ulen, v.ulen);
            swap(alen, v.alen);
        }
        else
        {
            growbuf(ulen+v.ulen);
            if(v.ulen) memcpy(&buf[ulen], (void  *)v.buf, v.ulen*sizeof(T));
            ulen += v.ulen;
            v.ulen = 0;
        }
    }
    
    bool inrange(size_t i) const
    {
        return i<size_t(ulen);
    }

    bool inrange(int i) const
    {
        return i>=0 && i<ulen;
    }

    T &pop()
    {
        return buf[--ulen];
    }
	
    T &last()
    {
        return buf[ulen-1];
    }

	void drop()
	{
		ulen--;
		buf[ulen].~T();
	}

    bool empty() const
    {
        return ulen==0;
    }

    int capacity() const
    {
        return alen;
    }
	
    int length() const
    {
        return ulen;
    }

    T &operator[](int i)
    {
        assert(i>=0 && i<ulen);
        return buf[i];
    }

    const T &operator[](int i) const
    {
        assert(i >= 0 && i<ulen);
        return buf[i];
    }

    void disown()
	{ 
		buf = NULL; 
		alen = ulen = 0;
	}
	
    void shrink(int i)
    {
        assert(i<=ulen);
        if(isclass<T>::no) ulen = i;
        else while(ulen>i) drop();
    }
    
    void setsize(int i)
    {
        assert(i<=ulen);
        ulen = i;
    }

    void deletecontents()
    {
        while(!empty())
            delete pop();
    }
    
    void deletearrays()
    {
        while(!empty())
            delete[] pop();
    }

    T *getbuf()
    {
        return buf;
    }
	
    const T *getbuf() const
    {
        return buf;
    }
    
    bool inbuf(const T *e) const
    {
        return e >= buf && e < &buf[ulen];
    }

	template<class F>
    void sort(F fun, int i = 0, int n = -1)
    {
        quicksort(&buf[i], n < 0 ? ulen-i : n, fun);
    }

    void sort()
    {
        sort(sortless());
    }
    
    void sortname()
    {
        sort(sortnameless());
    }

	void shuffle()
    {
    	for(int i = 0; i < ulen; i++){
    		int indx = rnd(ulen);
    		T temp = buf[i];
    		buf[i] = buf[indx];
    		buf[indx] = temp;
    	}
    }

    void growbuf(int sz)
    {
        int olen = alen;
        if(!alen) alen = std::max(MINSIZE, sz);
        else while(alen < sz) alen += alen/2;
        if(alen <= olen) return;
        buf = (T *)realloc(buf, alen*sizeof(T));
        if(!buf) abort();
    }

    databuf<T> reserve(int sz)
    {
        if(alen-ulen < sz) growbuf(ulen+sz);
        return databuf<T>(&buf[ulen], sz);
    }

    void advance(int sz)
    {
        ulen += sz;
    }

    void addbuf(const databuf<T> &p)
    {
        advance(p.length());
    }

    T *pad(int n)
    {
        T *buf = reserve(n).buf;
        advance(n);
        return buf;
    }

    void put(const T &v) 
	{
		add(v);
	}

    void put(const T *v, int n)
    {
        databuf<T> buf = reserve(n);
        buf.put(v, n);
        addbuf(buf);
    }

    void remove(int i, int n)
    {
        for(int p = i+n; p<ulen; p++) buf[p-n] = buf[p];
        ulen -= n;
    }

    T remove(int i)
    {
        T e = buf[i];
        for(int p = i+1; p<ulen; p++) buf[p-1] = buf[p];
        ulen--;
        return e;
    }

    T removeunordered(int i)
    {
        T e = buf[i];
        ulen--;
        if(ulen>0) buf[i] = buf[ulen];
        return e;
    }

    template<class U>
    int find(const U &o)
    {
        loopi(ulen) if(buf[i]==o) return i;
        return -1;
    }

    void addunique(const T &o)
    {
        if(find(o) < 0) add(o);
    }

    void removeobj(const T &o)
    {
        loopi(ulen) if(buf[i] == o)
        {
            int dst = i;
            for(int j = i+1; j < ulen; j++) if(!(buf[j] == o)) buf[dst++] = buf[j];
            setsize(dst);
            break;
        }
    }

    void replacewithlast(const T &o)
    {
        if(!ulen) return;
        loopi(ulen-1) if(buf[i]==o)
        {
            buf[i] = buf[ulen-1];
            break;
        }
        ulen--;
    }

    T &insert(int i, const T &e)
    {
        add(T());
        for(int p = ulen-1; p>i; p--) buf[p] = buf[p-1];
        buf[i] = e;
        return buf[i];
    }

    T *insert(int i, const T *e, int n)
    {
        if(alen-ulen < n) growbuf(ulen+n);
        loopj(n) add(T());
        for(int p = ulen-1; p>=i+n; p--) buf[p] = buf[p-n];
        loopj(n) buf[i+j] = e[j];
        return &buf[i];
    }

    void reverse()
    {
        loopi(ulen/2) swap(buf[i], buf[ulen-1-i]);
    }


    static int heapparent(int i)
    {
        return (i - 1) >> 1;
    }
    
    static int heapchild(int i)
    {
        return (i << 1) + 1;
    }

    void buildheap()
    {
        for(int i = ulen/2; i >= 0; i--) downheap(i);
    }

    int upheap(int i)
    {
        float score = heapscore(buf[i]);
        while(i > 0)
        {
            int pi = heapparent(i);
            if(score >= heapscore(buf[pi])) break;
            swap(buf[i], buf[pi]);
            i = pi;
        }
        return i;
    }

    T &addheap(const T &x)
    {
        add(x);
        return buf[upheap(ulen-1)];
    }

    int downheap(int i)
    {
        float score = heapscore(buf[i]);
        for(;;)
        {
            int ci = heapchild(i);
            if(ci >= ulen) break;
            float cscore = heapscore(buf[ci]);
            if(score > cscore)
            {
               if(ci+1 < ulen && heapscore(buf[ci+1]) < cscore) { swap(buf[ci+1], buf[i]); i = ci+1; }
               else { swap(buf[ci], buf[i]); i = ci; }
            }
            else if(ci+1 < ulen && heapscore(buf[ci+1]) < score) { swap(buf[ci+1], buf[i]); i = ci+1; }
            else break;
        }
        return i;
    }

    T removeheap()
    {
        T e = removeunordered(0);
        if(ulen) downheap(0);
        return e;
    }

    template<class K> 
    int htfind(const K &key)
    {
        loopi(ulen) if(htcmp(key, buf[i])) return i;
        return -1;
    }
};
