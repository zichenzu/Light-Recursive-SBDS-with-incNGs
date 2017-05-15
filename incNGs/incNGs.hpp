#include <gecode/int/branch.hh>
#include <gecode/int.hh>
#include <gecode/incNGs/head.hpp>

namespace Gecode{  
	 
	extern IntArgs cs_index, cs_eqval, cs_nqval, ex_cs_inN;
	extern int cs_ind, nSym, stackN, dosize,s_index;   
	extern int ex_nSym,ch,un_num;
	extern IntArgs ex_nSymR;
	extern IntArgs re_index_h; 
	extern IntArgs ex_check,unS,WatNum;

	void symGoal(int index, int val) {
	  
				
		// special for IntVars but could be generalised
		if (ex_nSym == 0) { return ;} // i.e. no goal 	
					
		else { 
			int i = 0; 
			Index_class k=(*_symmetries)(ex_nSymR[i],index,val);
		  
		 
		  
			for (; i < ex_nSym; i++) 
			{  

				k=(*_symmetries)(ex_nSymR[i],index,val);
				 
				if(ex_cs_inN[stackN*nSym+ex_nSymR[i]]<s_index && (k.index!=index || k.val!=val)){
					 
					 
					cs_index[ex_nSymR[i]*s_index+ex_cs_inN[stackN*nSym+ex_nSymR[i]]]=k.index;
					cs_eqval[ex_nSymR[i]*s_index+ex_cs_inN[stackN*nSym+ex_nSymR[i]]]=-1;
					cs_nqval[ex_nSymR[i]*s_index+ex_cs_inN[stackN*nSym+ex_nSymR[i]]]=k.val;
					ex_cs_inN[stackN*nSym+ex_nSymR[i]]++;
						 
					 
			 
			   }
			
			}
			  
		}
		
	};

			
				
	template<class View>
	 class IncNGs : public Propagator {
	 protected: 
		ViewArray<View> x;
		//SBDS_Sym SymObject;
		int csN;
		int pos_a;
		int pos_b;
		int pp;
		int** MyA;
		IntArgs re_index;
	 public: 
		//post
		IncNGs(Space& home,  ViewArray<View>& x0, int r);
		//copy
		IncNGs(Space& home, bool share, IncNGs<View>& p);
		virtual Propagator* copy(Space& home, bool share);
		//cost
		virtual PropCost cost(const Space&, const ModEventDelta&) const;
		//propagation
		virtual ExecStatus propagate(Space& home, const ModEventDelta&);
		//post
		static ExecStatus post(Space& home, ViewArray<View>& x0, int r);
		//dispose
		virtual size_t dispose(Space& home);
	};

		
		// posting
		template<class View>
		inline
		IncNGs<View>::IncNGs(Space& home,  ViewArray<View>& x0, int r) : Propagator(home), x(x0), csN(r){
			//initialize 
			pos_a=0;
			pp=-1;
			 
			pos_b=s_index;
			
			x.subscribe(home,*this,Int::PC_INT_DOM); 
			
			 
		} 
		template<class View>
		ExecStatus
		IncNGs<View>::post(Space& home, ViewArray<View>& x0, int r) {
			(void) new (home) IncNGs(home,x0,r); 
			return ES_OK; 
		} 
		// disposal 
		template<class View>
		forceinline size_t
		IncNGs<View>::dispose(Space& home) {
			//std::cout<<"cancel in here\n";
			x.cancel(home,*this,Int::PC_INT_DOM);
			
			(void) Propagator::dispose(home);
			return sizeof(*this); 
		} 
		// copying
		template<class View>
		forceinline
		IncNGs<View>::IncNGs(Space& home, bool share, IncNGs<View>& p) : Propagator(home,share,p),csN(p.csN),pp(p.pp),
			pos_a(p.pos_a),pos_b(p.pos_b){
			
			x.update(home,share,p.x); 
			
			
			//get the number of assignments
			int i=pos_a; 
			int n=ex_cs_inN[stackN*nSym+csN];
			if(n>s_index) n-=s_index;
			int k=0;
			/// update pos_a
			for(;i<n && i<pos_b;i++)
			{   
				if(cs_eqval[csN*s_index+i]==-1 && x[cs_index[csN*s_index+i]].in(cs_nqval[csN*s_index+i]))
				{   
				  
					WatNum[cs_index[csN*s_index+i]*dosize+cs_nqval[csN*s_index+i]]+=1;
					ch=1;
					k=1;
				}
				else if(cs_nqval[csN*s_index+i]==-1 && x[cs_index[csN*s_index+i]].in(cs_eqval[csN*s_index+i]))
				{   
				  
					WatNum[cs_index[csN*s_index+i]*dosize+cs_eqval[csN*s_index+i]]+=1;
					ch=1;
					k=1;
				}
			 
			}
		
			if(k==0) {
				unS[un_num++]=csN;
			}
		} 
		template<class View> Propagator*  IncNGs<View>::copy(Space& home, bool share) {
			return new (home) IncNGs(home,share,*this); 
		} 
		// cost computation 
		template<class View>
		PropCost
		IncNGs<View>::cost(const Space&, const ModEventDelta&) const {
			return PropCost::binary(PropCost::LO); 
		} 
		// propagation 
		template<class View>
		ExecStatus
		IncNGs<View>::propagate(Space& home, const ModEventDelta&) {

		
		if(ex_cs_inN[stackN*nSym+csN]==0) return ES_FIX;
		
			
		 
		{
			int n=ex_cs_inN[stackN*nSym+csN];
			if(n>s_index) n-=s_index;
			
			
		
			int i=pos_a; 
			/// update pos_a
			for(;i<n && i<pos_b && (cs_eqval[csN*s_index+i]==-1 || 
				(x[cs_index[csN*s_index+i]].assigned()&& 
				x[cs_index[csN*s_index+i]].val()==cs_eqval[csN*s_index+i]))
				;i++)
			{   
				if(cs_eqval[csN*s_index+i]==-1 && x[cs_index[csN*s_index+i]].in(cs_nqval[csN*s_index+i]))
				{   
				  
					 symGoal(cs_index[csN*s_index+i], cs_nqval[csN*s_index+i]);
					GECODE_ME_CHECK(x[cs_index[csN*s_index+i]].nq(home,cs_nqval[csN*s_index+i]));
				}
			 
			}
			
			 
			for(int g=0;g<x.size();g++)
				ex_check[g]=0;
			 
			int pos_c=-1;
			pos_a=i;
			///update pos_b
			int ps=pos_a;
			int ns=-1;
			int rps=pos_a;
			for(;i<n && i<pos_b;i++)
			{
				if(x[cs_index[csN*s_index+i]].in(cs_eqval[csN*s_index+i])) 
				{
					if(pos_c!=-1&&ex_check[cs_index[csN*s_index+i]]!=x[cs_index[csN*s_index+i]].size()-1)
						pos_c=-1;
					else if(pos_c==-1&& (ex_check[cs_index[csN*s_index+i]]==x[cs_index[csN*s_index+i]].size()-1))
					{
						pos_c=i;
						rps=ps;
					}
					ps=i;
					continue;
				}
				
				if(cs_eqval[csN*s_index+i]!=-1) break;
				if(x[cs_index[csN*s_index+i]].in(cs_nqval[csN*s_index+i]))
				{
					ns=i;
					ex_check[cs_index[csN*s_index+i]]++;
				}
				if(ex_check[cs_index[csN*s_index+i]]==x[cs_index[csN*s_index+i]].size())
					break;
			
			
			}
			
			int k=pos_b;
			if(i!=n && i!=pos_b) 
			{	
				 
				if(cs_eqval[csN*s_index+i]==-1)
				{
					pos_b=ps+1;
					if(pos_c!=-1)        
						pos_b=rps+1;
					if(pos_a==pos_b-1) { //std::cout<<"equal in here, then return subsumed  "<<pp<<"\n";
						ex_cs_inN[stackN*nSym+csN]+=2*s_index;

						symGoal(cs_index[csN*s_index+pos_a], cs_eqval[csN*s_index+pos_a]);
						GECODE_ME_CHECK(x[cs_index[csN*s_index+pos_a]].nq(home,cs_eqval[csN*s_index+pos_a]));
						return home.ES_SUBSUMED(*this);
					}
					pos_b=i+1;
				}
				else       
					pos_b=ns+1;
				 
			}
			
			if(pos_b==0)
			{
				ex_cs_inN[stackN*nSym+csN]+=2*s_index;
				return home.ES_SUBSUMED(*this);
			}
		   
			
		
			
			///symmetry is broken 
			if(k!=pos_b && k==s_index)
			{	 
				ex_cs_inN[stackN*nSym+csN]+=s_index;
			}
			
			 
			return ES_FIX;
		} 
			
	}


	void incNGs(Space& home,ViewArray<Int::IntView> x, int r) {

		//constraint post function 
	 
		if (IncNGs<Int::IntView>::post(home,x,r) != ES_OK)
			home.fail(); 
	} 
}
