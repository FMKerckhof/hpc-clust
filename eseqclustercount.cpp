#include "eseqcluster.h"
#include "cluster-common.h"

#include <eutils/efile.h>

eseqdistCount::eseqdistCount(): count(1) {}
eseqdistCount::eseqdistCount(INDTYPE _x,INDTYPE _y,float _dist): x(_x),y(_y),dist(_dist),count(1) {}


void eseqdistCount::serial(estr& data) const
{
//  ::serial(x,data); ::serial(y,data); ::serial(dist,data);
  ::serial(x,data); ::serial(y,data); ::serial(dist,data); ::serial(count,data);
}

int eseqdistCount::unserial(const estr& data,int i)
{
  i=::unserial(x,data,i);
  if (i==-1) return(i);
  i=::unserial(y,data,i);
  if (i==-1) return(i);
  i=::unserial(dist,data,i);
  if (i==-1) return(i);
  i=::unserial(count,data,i);
  return(i);
}



ostream& operator<<(ostream& stream,const eseqdistCount& sdist)
{
  stream << "("<<sdist.x<<","<<sdist.y<<","<<sdist.dist<<","<<sdist.count<<")";
  return(stream);
}



eseqclusterCount::eseqclusterCount(){}


void eseqclusterCount::check(ebasicarray<eseqdistCount>& dists)
{
  long i;
  estr xystr;
  bool duplicate=false;
  for (i=0; i<dists.size(); ++i){
    if (i%(dists.size()/10)==0) { cout << i*10/dists.size(); flush(cout); }
//    xy2estr(dists[i].x,dists[i].y,xystr);

    eseqdisthash::iter it;

//    cout << dists[i].dist << " " << dists[i].x << " " << dists[i].y;
//    it=smatrix.get(xystr);
    it=smatrix.get(dists[i]);
    if (it != smatrix.end()) {
      cout << "duplicate found: "+estr(dists[i].x)+","+dists[i].y << endl;
//      cout << " *";
      duplicate=true;
    }else
      smatrix.add(dists[i],1);
//    cout << endl;
  }
  smatrix.clear();

  ldieif(duplicate,"duplicates found");
  cout << "# no duplicates found!" << endl;
}

void eseqclusterCount::init(INDTYPE count) {
  long i;
  scount.reserve(count);
  scluster.reserve(count);
  smerge.reserve(count);
  inter.reserve(count);
  incluster.reserve(count);
  for (i=0; i<count; ++i){
    scount.add(1);
    scluster.add(i);
    smerge.add(-1);
    incluster.add(list<INDTYPE>());
    incluster[i].push_back(i);
    inter.add(list<INDTYPE>());
  }
  cout << "# initializing cluster with: "<< count<< " seqs" << endl; 
  cout << "# initializing smatrix with: " << (long int)(count)*(long int)(count)/20000l/2l<< " elements" << endl; 
  smatrix.reserve((long int)(count)*(long int)(count)/20000l/2l);
//  cout << "# smatrix._hashitems = " << smatrix._hashitems << endl;
  mergecount=0;
}

long eseqclusterCount::update(long s)
{
  long count=0;
  long i;
  long smergepos=0;
  ebasicarray<long> tmpsmerge;
  long updcount;
  long updind[smerge.size()];
  long updcount2;
  long updind2[smerge.size()];

  for (i=0; i<smerge.size(); ++i)
    tmpsmerge.add(-1);

  for (i=0; i<smerge.size(); ++i)
    updind2[i]=-1;

  updcount2=0;
  for (i=0; i<scluster.size(); ++i){
    if (updind2[scluster[i]]==-1){
      updind2[scluster[i]]=updcount2;
      ++updcount2;
    }
  }

  // make sure to only update 100 entries at a time, this will force more passes but use less memory
  do {
    updcount=0;

    for (i=0; i<smerge.size(); ++i)
      updind[i]=-1;

    for (i=0; i<tmpsmerge.size(); ++i)
      tmpsmerge[i]=-1;

    for (; smergepos<smerge.size(); ++smergepos){
      if (smerge[smergepos]>=0 && scluster[smergepos]!=smergepos){
        if (updind[scluster[smergepos]]==-1) {
          updind[scluster[smergepos]]=updcount;
          ++updcount;
        }
        updind[smergepos]=updind[scluster[smergepos]];
        tmpsmerge[smergepos]=scluster[smergepos];
        tmpsmerge[scluster[smergepos]]=scluster[smergepos];
//        ldieif(scluster[scluster[smergepos]]!=scluster[smergepos],"something wrong??");
        if (updcount==100) { ++smergepos; break; }
      }
/*
      if (smerge[smergepos]>=0){
        if (updind[smerge[smergepos]]==-1) {
          updind[smerge[smergepos]]=updcount;
          ++updcount;
        }
        updind[smergepos]=updind[smerge[smergepos]];
        tmpsmerge[smergepos]=smerge[smergepos];
//        if (updcount==100) break;
      }
*/
    }

    if (updcount==0) return(0);

    cerr << "# updating: " << updcount << " merges smerge.size: "<<tmpsmerge.size()<<endl;

    long *uarr=new long[updcount*updcount2];
    ldieif(uarr==0x00,"not enough memory");
    long li,lj;
    for (i=0; i<updcount*updcount2; ++i)
      uarr[i]=-1l;

    for (li=s; li>=0; --li){
      if (dists[li].count==0) continue;

      if (tmpsmerge[dists[li].x]>=0){
        lj=uarr[updind[tmpsmerge[dists[li].x]]*updcount2+updind2[scluster[dists[li].y]]];
        if (lj>=0){
          dists[li].count+=dists[lj].count;
          dists[lj].count=0;
          ++count;
        }
        uarr[updind[tmpsmerge[dists[li].x]]*updcount2+updind2[scluster[dists[li].y]]]=li;
      }else if (tmpsmerge[dists[li].y]>=0){
        lj=uarr[updind[tmpsmerge[dists[li].y]]*updcount2+updind2[scluster[dists[li].x]]];
        if (lj>=0){
          dists[li].count+=dists[lj].count;
          dists[lj].count=0;
          ++count;
        }
        uarr[updind[tmpsmerge[dists[li].y]]*updcount2+updind2[scluster[dists[li].x]]]=li;
      }
    }

    delete[] uarr;
  }while (updcount==100);

  for (i=0; i<smerge.size(); ++i)
    smerge[i]=-1;

  return(count);
}

long eseqclusterCount::update(long int s,INDTYPE x,INDTYPE y)
{
//  int count=0;
  long i,j;

  eintarray uarr;
  uarr.reserve(smerge.size());
  for (i=0; i<smerge.size(); ++i)
    uarr.add(-1);

  for (i=s; i>=0; --i){
    if (dists[i].count==0) continue;

    if (scluster[dists[i].x]==x || scluster[dists[i].x]==y){
      j=uarr[scluster[dists[i].y]];
      if (j>=0){
        dists[i].count+=dists[j].count;
        dists[j].count=0;
//        ++count;
      }
      uarr[scluster[dists[i].y]]=i;
    }else if (scluster[dists[i].y]==x || scluster[dists[i].y]==y){
      j=uarr[scluster[dists[i].x]];
      if (j>=0){
        dists[i].count+=dists[j].count;
        dists[j].count=0;
//        ++count;
      }
      uarr[scluster[dists[i].x]]=i;
    }
  }
  return(0);
//  return(count);
}

long eseqclusterCount::update(eblockarray<eseqdistCount>& dists,long s)
{
  long count=0;
  long i,j;
  long tmp;
  eseqdisthash cmatrix;
  eseqdisthash::iter it;
//  ebasicstrhashof<int> cmatrix;
//  ebasichashmap<estr,int>::iter it;
//  estr tmps; 

  for (i=s; i>=0; --i){
    if (dists[i].count==0) continue;

    if (smerge[dists[i].x]>=0 || smerge[dists[i].y]>=0){
      dists[i].x=scluster[dists[i].x];
      dists[i].y=scluster[dists[i].y];
      if (dists[i].x>dists[i].y) { tmp=dists[i].x; dists[i].x=dists[i].y; dists[i].y=tmp; }

//      xy2estr(scluster[dists[i].x],scluster[dists[i].y],tmps);
//      it=cmatrix.get(tmps);
      it=cmatrix.get(dists[i]);
      if (it==cmatrix.end())
//        cmatrix.add(tmps,i);
        cmatrix.add(dists[i],i);
      else{
        j=*it;
        dists[i].count+=dists[j].count;
        dists[j].count=0;
        *it=i;
        ++count;
      }
    }
  }

  for (i=0; i<smerge.size(); ++i)
    smerge[i]=-1;

  return(count);
}

void eseqclusterCount::merge(INDTYPE x,INDTYPE y,float dist)
{
  ldieif(x==y,"should not happen!");
  ldieif(scount[x]==0 || scount[y]==0,"also should not happen");

  clusterData.mergearr.add(eseqdist(x,y,dist));

  smerge[x]=x;
  smerge[y]=x;

  scount[x]+=scount[y];
  scount[y]=0;

  list<INDTYPE>::iterator it;
  for (it=incluster[y].begin(); it!=incluster[y].end(); ++it){
    scluster[*it]=x;
    incluster[x].push_back(*it);
  }

  estr tmpstr,tmpstr2;

  eseqdistCount tmpdist,tmpdist2;
  tmpdist.x=x;
  tmpdist2.x=y;
  long i,j;
  for (it=inter[y].begin(); it!=inter[y].end(); ++it){
    j=scluster[*it];
    if (x==j || y==j) continue;
    tmpdist.y=j;
    tmpdist2.y=j;
//    xy2estr(x,j,tmpstr);
//    xy2estr(y,j,tmpstr2);

//    ebasichashmap<estr,int>::iter tmpit2=smatrix.get(tmpstr2);
    eseqdisthash::iter tmpit2=smatrix.get(tmpdist2);
    if(tmpit2==smatrix.end()) continue;

//    ebasichashmap<estr,int>::iter tmpit=smatrix.get(tmpstr);
    eseqdisthash::iter tmpit=smatrix.get(tmpdist);
    if (tmpit!=smatrix.end())
      *tmpit+=*tmpit2;
    else{
      smatrix.add(tmpdist,*tmpit2);
      inter[x].push_back(j);
    }
    smatrix.erase(tmpit2);
  }
  ++mergecount;
}

void eseqclusterCount::add(eseqdistCount& sdist){
  if (sdist.count==0) return;
  ldieif(sdist.x<0 || sdist.y<0 || sdist.x>=scluster.size() || sdist.y>=scluster.size(),"out of bounds: sdist.x: "+estr(sdist.x)+" sdist.y: "+estr(sdist.y)+" scluster.size(): "+estr(scluster.size()));

  eseqdistCount tmpdist;
  tmpdist.x=scluster[sdist.x];
  tmpdist.y=scluster[sdist.y];
  tmpdist.count=sdist.count;
  tmpdist.dist=sdist.dist;

//  int x=scluster[sdist.x];
//  int y=scluster[sdist.y];

  ldieif(tmpdist.x<0 || tmpdist.y<0 || tmpdist.x>=scluster.size() || tmpdist.y>=scluster.size(),"out of bounds: sdist.x: "+estr(tmpdist.x)+" sdist.y: "+estr(tmpdist.y)+" scluster.size(): "+estr(scluster.size()));
  long tmp;
  if (tmpdist.x>tmpdist.y) { tmp=tmpdist.x; tmpdist.x=tmpdist.y; tmpdist.y=tmp; }

  long links;
  long i;
  estr xystr;

//  cout << x << " " << y << " " << sdist.dist << endl;
  ldieif(tmpdist.x==tmpdist.y,"should not happen: "+estr(tmpdist.x)+","+estr(tmpdist.y)+" --- "+estr(sdist.x)+","+estr(sdist.y));

//  xy2estr(x,y,xystr);

//  ebasichashmap<estr,int>::iter it;
  eseqdisthash::iter it;

//  it=smatrix.get(xystr);

  it=smatrix.get(tmpdist);
  if (it==smatrix.end()){
    if (scount[tmpdist.x]*scount[tmpdist.y]==tmpdist.count){
//    if (scount[x]*scount[y]==1){
      merge(tmpdist.x,tmpdist.y,tmpdist.dist);
//      update(ind-1,x,y);
      cout << scluster.size()-mergecount << " " << tmpdist.dist << " " << tmpdist.x << " " << tmpdist.y << endl;
//      sleep(1);
    }else{
      smatrix.add(tmpdist,sdist.count);
//      smatrix.add(xystr,sdist.count);
//      smatrix.add(xystr,1);
      inter[tmpdist.x].push_back(tmpdist.y); inter[tmpdist.y].push_back(tmpdist.x);
    }
    return;
  }

  (*it)+=tmpdist.count;
//  ++(*it);

  // complete linkage
  if ((*it)==scount[tmpdist.x]*scount[tmpdist.y]){
    merge(tmpdist.x,tmpdist.y,tmpdist.dist);
//    update(ind-1,x,y);
    cout << scluster.size()-mergecount << " " << tmpdist.dist << " " << tmpdist.x << " " << tmpdist.y << endl;
//    sleep(1);
//    cout << sdist.dist << " " << x << " " << y << endl;
    smatrix.erase(it);
  }
}

/*
void eseqclusterCount::add(int ind){
  if (dists[ind].count==0) return;
  ldieif(dists[ind].x<0 || dists[ind].y<0 || dists[ind].x>=scluster.size() || dists[ind].y>=scluster.size(),"out of bounds: sdist.x: "+estr(dists[ind].x)+" sdist.y: "+estr(dists[ind].y)+" scluster.size(): "+estr(scluster.size()));

  int x=scluster[dists[ind].x];
  int y=scluster[dists[ind].y];

  ldieif(x<0 || y<0 || x>=scluster.size() || y>=scluster.size(),"out of bounds: sdist.x: "+estr(x)+" sdist.y: "+estr(y)+" scluster.size(): "+estr(scluster.size()));
  int tmp;
  if (x>y) { tmp=x; x=y; y=tmp; tmp=dists[ind].x; dists[ind].x=dists[ind].y; dists[ind].y=tmp; }

  int links;
  int i;
  estr xystr;

//  cout << x << " " << y << " " << sdist.dist << endl;
  ldieif(x==y,"should not happen: "+estr(x)+","+estr(y)+" --- "+estr(dists[ind].x)+","+estr(dists[ind].y));

  xy2estr(x,y,xystr);

  ebasichashmap<estr,int>::iter it;

  it=smatrix.get(xystr);
  if (it==smatrix.end()){
    if (scount[x]*scount[y]==dists[ind].count){
//    if (scount[x]*scount[y]==1){
      merge(x,y);
//      update(ind-1,x,y);
      cout << scluster.size()-mergecount << " " << dists[ind].dist << " " << x << " " << y << endl;
//      sleep(1);
    }else{
      smatrix.add(xystr,dists[ind].count);
//      smatrix.add(xystr,1);
      inter[x].push_back(y); inter[y].push_back(x);
    }
    return;
  }

  (*it)+=dists[ind].count;
//  ++(*it);

  // complete linkage
  if ((*it)==scount[x]*scount[y]){
    merge(x,y);
//    update(ind-1,x,y);
    cout << scluster.size()-mergecount << " " << dists[ind].dist << " " << x << " " << y << endl;
//    sleep(1);
//    cout << sdist.dist << " " << x << " " << y << endl;
    smatrix.erase(it);
  }
}
*/

void eseqclusterCount::save(const estr& filename,const estrarray& arr)
{
  long i;
  estr otustr;
  estrhashof<ebasicarray<INDTYPE> > otus;

  efile f(filename+".clstr");
  for (i=0; i<scluster.size(); ++i)
    f.write(estr(scluster[i])+"     "+arr.keys(i)+"\n");
  f.close();

  list<INDTYPE>::iterator it;
  long otucount=0;
  efile f2(filename);
  for (i=0; i<incluster.size(); ++i){
    if (scount[i]==0) continue;
    f2.write(">OTU"+estr(otucount)+"\n");
    for (it=incluster[i].begin(); it!=incluster[i].end(); ++it)
      f2.write(arr.keys(*it)+"\n");
    ++otucount;
  }
  f2.close();
}

