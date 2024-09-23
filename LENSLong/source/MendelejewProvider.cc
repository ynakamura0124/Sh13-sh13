#include "MendelejewProvider.hh"
#include <string>
#include <iostream>

//convert capital letter to lowercase 	

const std::string MendelejewProvider::elements[] = {"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cp", "Uut", "Uuq", "Uup", "Uuh", "Uus", "Uuo"};


void MendelejewProvider::UpToLow(std::string &str)
{
	int i=1;
	while(str[i]!='\0')
	{
		if (str[i] >= 0x41 && str[i] <= 0x5A)
     		 str[i] = str[i] + 0x20;
     	i++;
	}
}

int MendelejewProvider::GetAtomicNumber(std::string atomicName)
{
	int atomicNumber(0);
	UpToLow(atomicName);
	for(int i=0; i<nrOfElements; i++)
		if(elements[i]==atomicName)
		{
			atomicNumber=i+1;
		}

	return atomicNumber;
}
