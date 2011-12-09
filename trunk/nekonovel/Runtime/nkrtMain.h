// ===============================================================
//  nkrtMain - 2009.7.15
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nkrtMain_h__
#define __nkrtMain_h__



namespace nekoRt
{
	class nkrtEventListener : public nekoIEventListener
	{
	public:

		nkrtEventListener() {}
		~nkrtEventListener() {}

		bool OnEvent(const nekoEvent &event);
	};
	class nkrtMain
	{
	protected:


	public:
		
		nkrtMain();
		virtual ~nkrtMain();
		
	};
}


#endif // __nkrtMain_h__