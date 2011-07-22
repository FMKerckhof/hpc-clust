#ifndef CLUSTER_COMMON_H
#define CLUSTER_COMMON_H

#include <eutils/evar.h>
#include <eutils/estr.h>
#include <eutils/ernd.h>
#include <eutils/ebasicarray.h>
#include <eutils/eintarray.h>
#include <eutils/ebasichashmap.h>
#include <eutils/estrhash.h>
#include <eutils/etimer.h>
#include <eutils/eblockarray.h>

#include <list>
#include <math.h>

#include <eutils/ethread.h>

#include "eseqcluster.h"
#include "eseqclustersingle.h"
#include "eseqclustercount.h"

inline void getL2(int& l,int w,int x,int y)
{
  if (y<x) { int t; t=x; x=y; y=t; }

  --w;
  if (x>w/2) { x=w-x; y-=1; l=x*w+y; return; }

  y-=x+1;
  l=x*w+y;
  return;
}

inline void getL(int& l,int w,int x,int y)
{
  --w;
  if (x>w/2) { x=w-x; y-=1; l=x*w+y; return; }

  y-=x+1;
  l=x*w+y;
  return;
}

inline void getXY(int l,int w,int& x,int& y)
{
  --w;
  x=(l/w);
  y=(l%w);
  if (y<w-x){ y+=x+1; return; }
  y+=1;
  x=w-x;
}

inline float dist_nogap(const estr& s1,const estr& s2,int gc1,int gc2)
{
  int i;
  int count=0;
  int gcount=0;
  for (i=0; i<s1._strlen; ++i){
    if (s1._str[i]==s2._str[i]) { if (s1._str[i]=='-') ++gcount; else ++count; }
  }
  return((float)count/(float)(s1._strlen-gc1-gc2+gcount));
}

const long int b4_m0=0x0Fl;
const long int b4_m1=0x0F0l;
const long int b4_m2=0x0F00l;
const long int b4_m3=0x0F000l;
const long int b4_m4=0x0F0000l;
const long int b4_m5=0x0F00000l;
const long int b4_m6=0x0F000000l;
const long int b4_m7=0x0F0000000l;
const long int b4_m8=0x0F00000000l;
const long int b4_m9=0x0F000000000l;
const long int b4_m10=0x0F0000000000l;
const long int b4_m11=0x0F00000000000l;
const long int b4_m12=0x0F000000000000l;
const long int b4_m13=0x0F0000000000000l;
const long int b4_m14=0x0F00000000000000l;
const long int b4_m15=0x0F000000000000000l;
const long int b4_umask=0x1111111111111111;

const long int b8_m0=0x00000000000000FFl;
const long int b8_m1=0x000000000000FF00l;
const long int b8_m2=0x0000000000FF0000l;
const long int b8_m3=0x00000000FF000000l;
const long int b8_m4=0x000000FF00000000l;
const long int b8_m5=0x0000FF0000000000l;
const long int b8_m6=0x00FF000000000000l;
const long int b8_m7=0xFF00000000000000l;

const long int b8_d0='-';
const long int b8_d1=b8_d0 << 8;
const long int b8_d2=b8_d0 << 16;
const long int b8_d3=b8_d0 << 24;
const long int b8_d4=b8_d0 << 32;
const long int b8_d5=b8_d0 << 40;
const long int b8_d6=b8_d0 << 48;
const long int b8_d7=b8_d0 << 56;


inline void dist_inc(long int a1,long int a2,long int mask,int& count){
  if ((a1&mask)==(a2&mask))
    ++count;
}

inline void dist_nogap_inc(long int a1,long int a2,long int mask,int& count,int& len){
  if ((a1&mask)==mask && (a2&mask)==mask)
    --len;
  else if ((a1&mask)==(a2&mask))
    ++count;
//  if ((a1&mask)==(a2&mask)){
//    if ((a1&mask)!=mask)
//      ++count;
//    else
//      --len;
//  }
}

//inline void dist_nogap_inc(long int a1,long int a2,long int mask,int& count,int& len){
//  if (!((a1&mask)==mask || (a2&mask)==mask)){
//    if ((a1&mask)==(a2&mask)) ++count;
//    ++len;
//  }
//}

inline void dist_tamura_inc(long int a1,long int a2,long int mask,int& P,int& Q,int& GC1,int& GC2,int& len){
  if ((a1&mask)==mask || (a2&mask)==mask)
    --len;
  else if (((a1^a2)&mask)==((0x02*b4_umask)&mask))
    ++P;
  else if (((a1^a2)&mask)==((0x03*b4_umask)&mask))
    ++Q;
  
  if ((a1&mask)==((0x02*b4_umask)&mask) || (a1&mask)==((0x03*b4_umask)&mask))
    ++GC1;

  if ((a2&mask)==((0x02*b4_umask)&mask) || (a2&mask)==((0x03*b4_umask)&mask))
    ++GC2;
}

inline float dist_tamura_compressed(const estr& s1,const estr& s2,int seqlen)
{
  int len=seqlen;
  int P=0; // transitions
  int Q=0; // transversions
  int GC1=0; // GC content seq1
  int GC2=0; // GC content seq2

  long int *ep1=(long int*)(s1._str)+(s1._strlen/8);
  long int *p1=(long int*)s1._str;
  long int *p2=(long int*)s2._str;
  for (; p1!=ep1; ++p1,++p2){
    dist_tamura_inc(*p1,*p2,b4_m0,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m1,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m2,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m3,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m4,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m5,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m6,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m7,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m8,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m9,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m10,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m11,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m12,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m13,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m14,P,Q,GC1,GC2,len);
    dist_tamura_inc(*p1,*p2,b4_m15,P,Q,GC1,GC2,len);
  }

  switch (seqlen%16){
   case 15:
    dist_tamura_inc(*p1,*p2,b4_m14,P,Q,GC1,GC2,len);
   case 14:
    dist_tamura_inc(*p1,*p2,b4_m13,P,Q,GC1,GC2,len);
   case 13:
    dist_tamura_inc(*p1,*p2,b4_m12,P,Q,GC1,GC2,len);
   case 12:
    dist_tamura_inc(*p1,*p2,b4_m11,P,Q,GC1,GC2,len);
   case 11:
    dist_tamura_inc(*p1,*p2,b4_m10,P,Q,GC1,GC2,len);
   case 10:
    dist_tamura_inc(*p1,*p2,b4_m9,P,Q,GC1,GC2,len);
   case 9:
    dist_tamura_inc(*p1,*p2,b4_m8,P,Q,GC1,GC2,len);
   case 8:
    dist_tamura_inc(*p1,*p2,b4_m7,P,Q,GC1,GC2,len);
   case 7:
    dist_tamura_inc(*p1,*p2,b4_m6,P,Q,GC1,GC2,len);
   case 6:
    dist_tamura_inc(*p1,*p2,b4_m5,P,Q,GC1,GC2,len);
   case 5:
    dist_tamura_inc(*p1,*p2,b4_m4,P,Q,GC1,GC2,len);
   case 4:
    dist_tamura_inc(*p1,*p2,b4_m3,P,Q,GC1,GC2,len);
   case 3:
    dist_tamura_inc(*p1,*p2,b4_m2,P,Q,GC1,GC2,len);
   case 2:
    dist_tamura_inc(*p1,*p2,b4_m1,P,Q,GC1,GC2,len);
   case 1:
    dist_tamura_inc(*p1,*p2,b4_m0,P,Q,GC1,GC2,len);
  }

  float C=GC1/seqlen + GC2/seqlen - 2.0*GC1/seqlen*GC2/seqlen;
  return(1+1.0*C*log(1.0-(float)P/(C*len)-(float)Q/len) + 0.5*(1.0-C)*log(1.0-2.0*Q/len));
}

inline float dist_compressed(const estr& s1,const estr& s2,int seqlen)
{
  int len=seqlen;
  int count=0;
  long int *ep1=(long int*)(s1._str)+(s1._strlen/8);
  long int *p1=(long int*)s1._str;
  long int *p2=(long int*)s2._str;
  for (; p1!=ep1; ++p1,++p2){
    dist_inc(*p1,*p2,b4_m0,count);
    dist_inc(*p1,*p2,b4_m1,count);
    dist_inc(*p1,*p2,b4_m2,count);
    dist_inc(*p1,*p2,b4_m3,count);
    dist_inc(*p1,*p2,b4_m4,count);
    dist_inc(*p1,*p2,b4_m5,count);
    dist_inc(*p1,*p2,b4_m6,count);
    dist_inc(*p1,*p2,b4_m7,count);
    dist_inc(*p1,*p2,b4_m8,count);
    dist_inc(*p1,*p2,b4_m9,count);
    dist_inc(*p1,*p2,b4_m10,count);
    dist_inc(*p1,*p2,b4_m11,count);
    dist_inc(*p1,*p2,b4_m12,count);
    dist_inc(*p1,*p2,b4_m13,count);
    dist_inc(*p1,*p2,b4_m14,count);
    dist_inc(*p1,*p2,b4_m15,count);
  }

  switch (seqlen%16){
   case 15:
    dist_inc(*p1,*p2,b4_m14,count);
   case 14:
    dist_inc(*p1,*p2,b4_m13,count);
   case 13:
    dist_inc(*p1,*p2,b4_m12,count);
   case 12:
    dist_inc(*p1,*p2,b4_m11,count);
   case 11:
    dist_inc(*p1,*p2,b4_m10,count);
   case 10:
    dist_inc(*p1,*p2,b4_m9,count);
   case 9:
    dist_inc(*p1,*p2,b4_m8,count);
   case 8:
    dist_inc(*p1,*p2,b4_m7,count);
   case 7:
    dist_inc(*p1,*p2,b4_m6,count);
   case 6:
    dist_inc(*p1,*p2,b4_m5,count);
   case 5:
    dist_inc(*p1,*p2,b4_m4,count);
   case 4:
    dist_inc(*p1,*p2,b4_m3,count);
   case 3:
    dist_inc(*p1,*p2,b4_m2,count);
   case 2:
    dist_inc(*p1,*p2,b4_m1,count);
   case 1:
    dist_inc(*p1,*p2,b4_m0,count);
  }
  return((float)count/(float)seqlen);
}

inline float dist_nogap_compressed(const estr& s1,const estr& s2,int seqlen)
{
  int len=seqlen;
  int count=0;
  long int *ep1=(long int*)(s1._str)+(s1._strlen/8);
  long int *p1=(long int*)s1._str;
  long int *p2=(long int*)s2._str;
  for (; p1!=ep1; ++p1,++p2){
    dist_nogap_inc(*p1,*p2,b4_m0,count,len);
    dist_nogap_inc(*p1,*p2,b4_m1,count,len);
    dist_nogap_inc(*p1,*p2,b4_m2,count,len);
    dist_nogap_inc(*p1,*p2,b4_m3,count,len);
    dist_nogap_inc(*p1,*p2,b4_m4,count,len);
    dist_nogap_inc(*p1,*p2,b4_m5,count,len);
    dist_nogap_inc(*p1,*p2,b4_m6,count,len);
    dist_nogap_inc(*p1,*p2,b4_m7,count,len);
    dist_nogap_inc(*p1,*p2,b4_m8,count,len);
    dist_nogap_inc(*p1,*p2,b4_m9,count,len);
    dist_nogap_inc(*p1,*p2,b4_m10,count,len);
    dist_nogap_inc(*p1,*p2,b4_m11,count,len);
    dist_nogap_inc(*p1,*p2,b4_m12,count,len);
    dist_nogap_inc(*p1,*p2,b4_m13,count,len);
    dist_nogap_inc(*p1,*p2,b4_m14,count,len);
    dist_nogap_inc(*p1,*p2,b4_m15,count,len);
  }

  switch (seqlen%16){
   case 15:
    dist_nogap_inc(*p1,*p2,b4_m14,count,len);
   case 14:
    dist_nogap_inc(*p1,*p2,b4_m13,count,len);
   case 13:
    dist_nogap_inc(*p1,*p2,b4_m12,count,len);
   case 12:
    dist_nogap_inc(*p1,*p2,b4_m11,count,len);
   case 11:
    dist_nogap_inc(*p1,*p2,b4_m10,count,len);
   case 10:
    dist_nogap_inc(*p1,*p2,b4_m9,count,len);
   case 9:
    dist_nogap_inc(*p1,*p2,b4_m8,count,len);
   case 8:
    dist_nogap_inc(*p1,*p2,b4_m7,count,len);
   case 7:
    dist_nogap_inc(*p1,*p2,b4_m6,count,len);
   case 6:
    dist_nogap_inc(*p1,*p2,b4_m5,count,len);
   case 5:
    dist_nogap_inc(*p1,*p2,b4_m4,count,len);
   case 4:
    dist_nogap_inc(*p1,*p2,b4_m3,count,len);
   case 3:
    dist_nogap_inc(*p1,*p2,b4_m2,count,len);
   case 2:
    dist_nogap_inc(*p1,*p2,b4_m1,count,len);
   case 1:
    dist_nogap_inc(*p1,*p2,b4_m0,count,len);
  }
  return((float)count/(float)len);
}


inline void dist_nogap_inc_dash(long int a1,long int a2,long int mask,long int dash,int& count,int& len){
  if ((a1&mask)==(a2&mask)){
    if ((a1&mask)!=mask)
      ++count;
    else
      --len;
  }
}

//inline void dist_nogap_inc_dash(long int a1,long int a2,long int mask,long int dash,int& count,int& len){
//  if (!((a1&mask)==dash || (a2&mask)==dash)){
//    if ((a1&mask)==(a2&mask)) ++count;
//    ++len;
//  }
//}

inline float dist_nogap(const estr& s1,const estr& s2)
{
  int count=0;
  int len=s1._strlen;
  long int *ep1=((long int*)s1._str)+(s1._strlen/8);
  long int *p1=(long int*)s1._str;
  long int *p2=(long int*)s2._str;
  for (; p1!=ep1; ++p1,++p2){
    dist_nogap_inc_dash(*p1,*p2,b8_m0,b8_d0,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m1,b8_d1,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m2,b8_d2,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m3,b8_d3,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m4,b8_d4,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m5,b8_d5,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m6,b8_d6,count,len);
    dist_nogap_inc_dash(*p1,*p2,b8_m7,b8_d7,count,len);
  }

  switch (s1._strlen%8){
   case 7:
    dist_nogap_inc_dash(*p1,*p2,b8_m6,b8_d6,count,len);
   case 6:
    dist_nogap_inc_dash(*p1,*p2,b8_m5,b8_d5,count,len);
   case 5:
    dist_nogap_inc_dash(*p1,*p2,b8_m4,b8_d4,count,len);
   case 4:
    dist_nogap_inc_dash(*p1,*p2,b8_m3,b8_d3,count,len);
   case 3:
    dist_nogap_inc_dash(*p1,*p2,b8_m2,b8_d2,count,len);
   case 2:
    dist_nogap_inc_dash(*p1,*p2,b8_m1,b8_d1,count,len);
   case 1:
    dist_nogap_inc_dash(*p1,*p2,b8_m0,b8_d0,count,len);
  }
  return((float)count/(float)len);
}

/*
inline float dist_nogap(const estr& s1,const estr& s2)
{
  int i;
  int count=0;
  int len=0;
  for (i=0; i<s1._strlen; ++i){
    if (s1._str[i]=='-' || s2._str[i]=='-') continue;
    if (s1._str[i]==s2._str[i]) ++count;
    ++len;
  }
  return((float)count/(float)len);
}
*/

inline float dist(const estr& s1,const estr& s2)
{
  int i;
  int count=0;
  for (i=0; i<s1.len(); ++i){
    if (s1._str[i]==s2._str[i]) ++count;
  }
  return((float)count/(float)s1.len());
}

inline float cooc_dist(int i,int j,earray<eintarray>& neigharr)
{
  int k;

  int cooc1,cooc2,common;

  cooc1=0; cooc2=0; common=0;
  if (neigharr[i].size()==0 || neigharr[j].size()==0) return(2.0);
  ldieif(j>=neigharr.size(),"j larger than neigharr.size()? "+estr(j)+">"+estr(neigharr.size()));
  ldieif(i>=neigharr.size(),"i larger than neigharr.size()? "+estr(i)+">"+estr(neigharr.size()));
  ldieif(neigharr[i].size() != neigharr[j].size(),"size mismatch: "+estr(neigharr[i].size())+" != "+estr(neigharr[j].size()));

  for (k=0; k<neigharr[i].size(); ++k){
    if (neigharr[i][k]!=0){ ++cooc1; if (neigharr[j][k]!=0) ++common; }
    if (neigharr[j][k]!=0){ ++cooc2; }
  }
  return(1.0-(float)common/(cooc1+cooc2-common));
}

inline void xy2estr(int x,int y,estr& str)
{
  str.clear();
  if (x<y){
    serialint(x,str);
    serialint(y,str);
  }else{
    serialint(y,str);
    serialint(x,str);
  }
}

template <class T,class M,class K,float (*fdist)(const estr&,const estr&,int)>
int t_calc_dists_noise(emutex& mutex,T& arr,K& dists,int seqlen,int node,int tnodes,float thres)
{
  ernd rng;
  float noise=0.025;
  long int i,i2,j;
  long int start,end;

  start=(long int)(node)*(long int)(arr.size()-1)/(long int)(2*tnodes);
  end=(long int)(node+1)*(long int)(arr.size()-1)/(long int)(2*tnodes);

  float tmpid,tmpid2,tmpid3;
  K tmpdists;

  for (i=start; i<end; ++i){
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>1.0) tmpid=1.0; else if (tmpid<0.0) tmpid=0.0;
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
    i2=arr.size()-i-2;
    for (j=i2+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i2],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>1.0) tmpid=1.0; else if (tmpid<0.0) tmpid=0.0;
      if (tmpid>=thres) tmpdists.add(M(i2,j,tmpid));
    }
  }
  if (node==tnodes-1 && arr.size()%2==0){
    i=arr.size()/2-1;
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>1.0) tmpid=1.0; else if (tmpid<0.0) tmpid=0.0;
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
  }
  mutex.lock();
  dists+=tmpdists;
  mutex.unlock();
  return(tmpdists.size());
}


template <class T,class M,class K,float (*fdist)(const estr&,const estr&,int)>
int t_calc_dists(emutex& mutex,T& arr,K& dists,int seqlen,int node,int tnodes,float thres)
{
  long int i,i2,j;
  long int start,end;

  start=(long int)(node)*(long int)(arr.size()-1)/(long int)(2*tnodes);
  end=(long int)(node+1)*(long int)(arr.size()-1)/(long int)(2*tnodes);

  float tmpid,tmpid2,tmpid3;
  K tmpdists;

  for (i=start; i<end; ++i){
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
    i2=arr.size()-i-2;
    for (j=i2+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i2],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i2,j,tmpid));
    }
  }
  if (node==tnodes-1 && arr.size()%2==0){
    i=arr.size()/2-1;
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
  }
  mutex.lock();
  dists+=tmpdists;
  mutex.unlock();
  return(0);
}

template <class T,class M,class K,float (*fdist)(const estr&,const estr&,int)>
void t_calc_dists_noise2(T& arr,K& tmpdists,int seqlen,int node,int tnodes,float thres)
{
  float noise=0.025;
  long int i,i2,j;
  long int start,end;
  ernd rng;

  start=(long int)(node)*(long int)(arr.size()-1)/(long int)(2*tnodes);
  end=(long int)(node+1)*(long int)(arr.size()-1)/(long int)(2*tnodes);

  float tmpid,tmpid2,tmpid3;

  for (i=start; i<end; ++i){
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>=thres) { if (tmpid>1.0) tmpid=1.0; tmpdists.add(M(i,j,tmpid)); }
    }
    i2=arr.size()-i-2;
    for (j=i2+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i2],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>=thres) { if (tmpid>1.0) tmpid=1.0; tmpdists.add(M(i2,j,tmpid)); }
    }
  }
  if (node==tnodes-1 && arr.size()%2==0){
    i=arr.size()/2-1;
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen)+rng.gaussian(noise);
      if (tmpid>=thres) { if (tmpid>1.0) tmpid=1.0; tmpdists.add(M(i,j,tmpid)); }
    }
  }
}

template <class T,class M,class K,float (*fdist)(const estr&,const estr&,int)>
void t_calc_dists2(T& arr,K& tmpdists,int seqlen,int node,int tnodes,float thres)
{
  long int i,i2,j;
  long int start,end;

  start=(long int)(node)*(long int)(arr.size()-1)/(long int)(2*tnodes);
  end=(long int)(node+1)*(long int)(arr.size()-1)/(long int)(2*tnodes);

  float tmpid,tmpid2,tmpid3;

  for (i=start; i<end; ++i){
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
    i2=arr.size()-i-2;
    for (j=i2+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i2],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i2,j,tmpid));
    }
  }
  if (node==tnodes-1 && arr.size()%2==0){
    i=arr.size()/2-1;
    for (j=i+1; j<arr.size(); ++j){
      tmpid=fdist(arr[i],arr[j],seqlen);
      if (tmpid>=thres) tmpdists.add(M(i,j,tmpid));
    }
  }
}



//int calc_dists_nogap(estrarray& arr,ebasicarray<eseqdist>& dists,int node,int tnodes,float thres);
//int calc_dists_nogap(estrarray& arr,earray<eseqdist>& dists,int node,int tnodes,float thres);
//int calc_dists_nogap(estrarray& arr,eintarray& arrgaps,earray<eseqdist>& dists,int node,int tnodes,float thres);

//int calc_dists_local(estrarray& arr,ebasicarray<float>& dists,float thres);

void cluster_init(earray<eintarray>& cvec,const estrhashof<int>& arrind,const eintarray& otuarr,int otucount);

void cooc_init(earray<eintarray>& neigharr,const estrarray& arr,const eintarray& otuarr,estrhashof<eintarray>& samples,estrhash& accsample,estrhashof<int>& arrind,int otucount);
void cooc_calc(int start,int end,ebasicarray<float>& dist_mat,earray<eintarray>& neigharr);
void cooc_calc2(int start,int end,ebasicarray<float>& dist_mat,earray<eintarray>& neigharr);

//void load_accs(const estr& filename,estrhash& arr);
//void load_seqs(const estr& filename,estrhash& arr);
void load_accs(const estr& filename,estrarray& arr);
void load_seqs(const estr& filename,estrarray& arr);


void load_seqs_mutate_compressed(const estr& filename,estrarray& arr,int& seqlen,float avgmutseq);

void load_seqs_compressed(const estr& filename,earray<estr>& arr,int& seqlen);
void load_seqs_compressed(const estr& filename,estrarray& arr,int& seqlen);
void load_seqs_compressed(const estr& filename,estrarray& arr,estrhashof<int>& arrind,int& seqlen);
void load_seqs(const estr& filename,estrarray& arr,eintarray& arrgaps);
void load_seqs(const estr& filename,estrhash& arr);
void load_seqs(const estr& filename,estrhashof<int>& arrind);
void load_seqs(const estr& filename,estrarray& arr,estrhashof<int>& arrind);

void load_samples(const estr& filename,estrhashof<int>& arrind,estrhashof<eintarray>& samples,estrhash& accsample);
//void load_samples(const estr& filename,estrhashof<earray<estr> >& samples,estrhash& accsample);

void load_clusters(const estr& filename,estrhashof<int>& arrind,eintarray& otuarr,int& otucount);
void load_clusters(const estr& filename,estrhashof<int>& arrind,earray<eintarray>& otus);

//void cluster_complete(const estrhash& arr,const ebasicarray<float>& dist_mat,const eintarray& dsort,earray<eintarray>& otus,eintarray& otuarr,float t);
//void cluster_single(const estrhash& arr,const ebasicarray<float>& dist_mat,const eintarray& dsort,earray<eintarray>& otus,float t);


#endif /* CLUSTER_COMMON_H */

