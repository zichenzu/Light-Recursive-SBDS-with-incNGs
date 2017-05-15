
//***********************************************************************//

#include <gecode/int/branch.hh>
#include <gecode/int.hh>
#include <gecode/incNGs/incNGs.hpp>
 
namespace Gecode {
	 
	 
	int nNum;
	 
	//the following are used for cs_sbds
	IntArgs cs_index, cs_eqval, cs_nqval;
	IntArgs ex_cs_inN;
	IntArgs ex_check,WatNum,unS;
	int cs_ind, ch,un_num;
	int nSym, stackN, dosize;
	   
	int s_index;
	int ex_nSym;
	IntArgs ex_nSymR;
	IntArgs re_index_h; 
	 

	void LReSBDS_Sym::symGoal(ViewArray<Int::IntView>& vars, int index, int val) {

		// special for IntVars but could be generalised
		if (_nSym == 0) { return ;} // i.e. no goal 	
				   
		else { 
			int i = 0; 
			Index_class k=(*_symmetries)(_nSymR[i],index,val);
	 
	  
			for (; i < _nSym; i++) 
			{  

				k=(*_symmetries)(_nSymR[i],index,val);
					 
				 //std::cout<<" for all sym "<<_nSymR[i]<<"\t"<<ex_cs_inN[mystack*nSym+_nSymR[i]]<<"\n";
				if(vars[k.index].in(k.val) &&ex_cs_inN[mystack*nSym+_nSymR[i]]<s_index && (k.index!=index || k.val!=val)){
					 
					cs_index[_nSymR[i]*s_index+ex_cs_inN[mystack*nSym+_nSymR[i]]]=k.index;
					cs_eqval[_nSymR[i]*s_index+ex_cs_inN[mystack*nSym+_nSymR[i]]]=-1;
					cs_nqval[_nSymR[i]*s_index+ex_cs_inN[mystack*nSym+_nSymR[i]]]=k.val;
				 
					ex_cs_inN[mystack*nSym+_nSymR[i]]++;
					 
	 
			   }
			 
			}
		  
		}
	};

		 
	LReSBDS_Sym::LReSBDS_Sym(Space& home, int nSym,int var_n,int dosize0) :
						_home(home),
						_nSym(nSym)
	   
		
		
		{
			dosize=dosize0;
	   
			_nSymR=IntArgs(_nSym);
		 
			 
			s_index=var_n*dosize;
			cs_ind=s_index*nSym;

			
			cs_index=IntArgs(cs_ind);
			cs_eqval=IntArgs(cs_ind);
			cs_nqval=IntArgs(cs_ind);
			ex_cs_inN=IntArgs(nSym*(var_n+1));
			re_index_h=IntArgs(var_n);
			ex_check=IntArgs(var_n);

			 

			for(int i=0;i<_nSym;i++)
				_nSymR[i]=i;

			for(int i=0;i<cs_ind;i++)
				cs_nqval[i]=cs_index[i]=cs_eqval[i]=-1;

			 
			for(int i=0;i<nSym;i++)
				 ex_cs_inN[i]=0;
			mystack=0;
			stackN=mystack;

		};
	LReSBDS_Sym::LReSBDS_Sym(Space& home,LReSBDS_Sym OldSym) :
						_nSym(OldSym.getNSym()),
						_home(home),
						mystack(OldSym.mystack),_nSymR(OldSym.getNSym())
	{ 
		for(int i=0;i<_nSym;i++)
		{	
		  _nSymR[i]=OldSym._nSymR[i];
		  ex_cs_inN[(OldSym.mystack+1)*nSym+_nSymR[i]]=ex_cs_inN[OldSym.mystack*nSym+_nSymR[i]];
		  
		} 

	};
	void LReSBDS_Sym::SymAdjust(ViewArray<Int::IntView>& vars, 
								int index, int val) 
	{
		 int oldNSym = _nSym; 


		_nSym = 0;
		 


		for (int i = 0 ; i < oldNSym ; i++) {
			if (ex_cs_inN[mystack*nSym+_nSymR[i]]<2*s_index) 
			{
				if (ex_cs_inN[mystack*nSym+_nSymR[i]]<s_index){	
					int ex=ex_cs_inN[mystack*nSym+_nSymR[i]];
					 
					Index_class k=(*_symmetries)(_nSymR[i],index,val);
					if(k.index!=index || k.val!=val){
						 
							cs_index[_nSymR[i]*s_index+ex]=k.index;
							cs_eqval[_nSymR[i]*s_index+ex]=k.val;
							cs_nqval[_nSymR[i]*s_index+ex]=-1;
							//std::cout<<"=========index  eqval :  "<<k.index<<"\t"<<k.val<<"\n";
							ex_cs_inN[mystack*nSym+_nSymR[i]]++;
						 //std::cout<<"= "<<_nSymR[i]<<"\t"<<ex<<"\t"<<k.index<<"\n";
					}  
				}
				_nSymR[_nSym++]=_nSymR[i];

				 
			}


		}
				 
	};



	//**************************************************//
	//**************************************************//
	//**************************************************//
	//**************************************************//
	//**************************************************//
	//*************   branch ***************************//



	 template<class View, int n, class Val, unsigned int a>
	class LReSBDSBrancher : public ViewValBrancher<View,n,Val,a> {
		typedef typename ViewBrancher<View,n>::BranchFilter BranchFilter;
	  public:
		/// Array of symmetry implementations
		LReSBDS_Sym SymObject;
		int start;
	  protected:
		/// Constructor for cloning \a b
		LReSBDSBrancher(Space& home, bool share, LReSBDSBrancher& b);
		/// Constructor for creation
		
		
		LReSBDSBrancher(Home home, 
					 ViewArray<View>& x,
					 ViewSel<View>* vs[n], 
					 ValSelCommitBase<View,Val>* vsc,
					 int nSym, int dosize0,
					 BranchFilter bf,IntVarValPrint vvp);
	  public:
		/// Return choice
		virtual const Choice* choice(Space& home);
		/// Return choice
		virtual const Choice* choice(const Space& home, Archive& e);
		/// Perform commit for choice \a c and alternative \a b
		virtual ExecStatus commit(Space& home, const Choice& c, unsigned int b);
		/// Perform cloning
		virtual Actor* copy(Space& home, bool share);
		/// Perform dispose
		virtual size_t dispose(Space& home);
		/// Delete brancher and return its size
		static BrancherHandle post(Home home,
								   ViewArray<View>& x,
								   ViewSel<View>* vs[n],
								   ValSelCommitBase<View,Val>* vsc,
								   int nSym, int dosize0,
								   BranchFilter bf,IntVarValPrint vvp);
	  };

	  template<class View, int n, class Val, unsigned int a> 
	  LReSBDSBrancher<View,n,Val,a>
	  ::LReSBDSBrancher(Home home, ViewArray<View>& x,
					 ViewSel<View>* vs[n],
					 ValSelCommitBase<View,Val>* vsc,
					 int nSym, int dosize0,
					 BranchFilter bf,IntVarValPrint vvp)
		: ViewValBrancher<View,n,Val,a>(home, x, vs, vsc, bf,vvp),
		  SymObject(home,nSym,x.size(),dosize0)
	  {	
			start=0;
			home.notice(*this, AP_DISPOSE);
			for(int i=0;i<nSym;i++)	incNGs(home,x,i);
		  
	  }

	  template<class View, int n, class Val, unsigned int a>
	  forceinline BrancherHandle
	  LReSBDSBrancher<View,n,Val,a>::
	  post(Home home, ViewArray<View>& x,
		   ViewSel<View>* vs[n], ValSelCommitBase<View,Val>* vsc,
		   int nSym,int dosize0,
		   BranchFilter bf,IntVarValPrint vvp) {
			return *new (home) LReSBDSBrancher<View,n,Val,a>(home,x,vs,vsc,nSym,dosize0,bf,vvp);
	  }

	  template<class View, int n, class Val, unsigned int a>
	  forceinline
	  LReSBDSBrancher<View,n,Val,a>::
	  LReSBDSBrancher(Space& home, bool shared, LReSBDSBrancher<View,n,Val,a>& b)
		: ViewValBrancher<View,n,Val,a>(home,shared,b), 
		  SymObject(home,b.SymObject) {
		  
			b.SymObject.mystack++;
			//std::cout<<"the mystack is "<<b.SymObject.mystack<<"\n";
			start=b.start;
			//std::cout<<"copy over\n";
			

	  }
	  
	  template<class View, int n, class Val, unsigned int a>
	  Actor*
	  LReSBDSBrancher<View,n,Val,a>::copy(Space& home, bool shared) {
		//std::cout<<"copy now! \n";
		return new (home) LReSBDSBrancher<View,n,Val,a>(home,shared,*this);
	  }


	  // Compute choice
	  template<class View, int n, class Val, unsigned int a>
	  const Choice*
	  LReSBDSBrancher<View,n,Val,a>::choice(Space& home) {  
	 
	 
	 
		return ViewValBrancher<View,n,Val,a>::choice(home);
	 
	  }

	 template<class View, int n, class Val, unsigned int a>
	  const Choice*
	  LReSBDSBrancher<View,n,Val,a>::choice(const Space& home, Archive& e) {

	  
		return ViewValBrancher<View,n,Val,a>::choice(home,e);
	  } 
	template<class View, int n, class Val, unsigned int a>
	  size_t
	  LReSBDSBrancher<View,n,Val,a>::dispose(Space& home) {
		home.ignore(*this,AP_DISPOSE);
		(void) ViewValBrancher<View,n,Val,a>::dispose(home);
		return sizeof(LReSBDSBrancher<View,n,Val,a>);
	  }
	  template<class View, int n, class Val, unsigned int a>
	  ExecStatus
	  LReSBDSBrancher<View,n,Val,a>
	  ::commit(Space& home, const Choice& c, unsigned int b) {
	  
			const PosValChoice<Val>& pvc
			  = static_cast<const PosValChoice<Val>&>(c);
			int pos = pvc.pos().pos;
			int val = pvc.val();
	 
			stackN=SymObject.mystack;  
			ex_nSym=SymObject._nSym; 
			for(int i=0;i<ex_nSym;i++)
				ex_nSymR[i]=SymObject._nSymR[i];
		
	 
			if(b==0)
			{     
				SymObject.SymAdjust(this->x,pos,val);//ajust symmetries
				ExecStatus fromBase = ViewValBrancher<View,n,Val,a>::commit(home, c, b);
				GECODE_ES_CHECK(fromBase);
	 
			}
			else
			{
				//symmetry breaking constraint
				SymObject.symGoal(this->x,pos, val);
				ExecStatus fromBase = ViewValBrancher<View,n,Val,a>::commit(home, c, b);
				GECODE_ES_CHECK(fromBase);
	 
			}
	 
		return ES_OK;
	  }
	  
	//**************************************************//
	//**************************************************//
	//**************************************************//
	//**************************************************//
	//**************************************************//
	//*************   post ***************************//

	 BrancherHandle
	  branch(Home home, const IntVarArgs& x,
			 IntVarBranch vars, IntValBranch vals,int _nSym,
			 SymmetryFunction symfunctions,int dosize0, IntBranchFilter bf=NULL) {
		using namespace Int;
		if (home.failed()) return BrancherHandle();
		ViewArray<IntView> xv(home,x);
		ViewSel<IntView>* vs[1] = { 
		  Branch::viewselint(home,vars) 
		};
		nSym=_nSym;
		//std::cout<<nSym<<"\n";
	 
		_symmetries=symfunctions;
		//post the cs_sbds constraint
		ex_nSym=nSym; 
		ex_nSymR=IntArgs(ex_nSym);
		unS=IntArgs(nSym);
		WatNum=IntArgs(x.size()*dosize0);
	 

		return LReSBDSBrancher<IntView,1,int,2>::post
			(home,xv,vs,Branch::valselcommitint(home,x.size(),vals),nSym,dosize0,bf,NULL);
		}
	  
}


  
