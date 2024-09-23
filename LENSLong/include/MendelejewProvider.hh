//source: http://www.cerim.pl/index.php?page=content/poradnik/pierwiastki.php
#include <string>
#ifndef MendelejewProvider_h
#define MendelejewProvider_h 1


class MendelejewProvider
{
	public:
	static int GetAtomicNumber(std::string name);
	static void UpToLow(std::string &str);
	private:
	
	MendelejewProvider();
	~MendelejewProvider();

	static const std::string elements[];// = {"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cp", "Uut", "Uuq", "Uup", "Uuh", "Uus", "Uuo"};
	static const int nrOfElements = 118;

};
#endif //MendelejewProvider
