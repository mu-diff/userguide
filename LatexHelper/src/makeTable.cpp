#include<iostream>
#include <fstream>
#include <algorithm>    // std::sort
#include<vector>
#include<string>

class MudiffFun{
public:
	MudiffFun(){ _id = Nb_of_Fun; _name = "none";  _folder = "none"; _summary = "none"; Nb_of_Fun++; }
	static int Nb_of_Fun;
	int _id;
	std::string _name;
	std::string _summary;
	std::string _folder;
	
	static bool SortByName (const MudiffFun& mufun1, const MudiffFun& mufun2)
	{
		std::string comp1 = mufun1._name, comp2 = mufun2._name;
		std::transform(comp1.begin(), comp1.end(), comp1.begin(), ::tolower);
		std::transform(comp2.begin(), comp2.end(), comp2.begin(), ::tolower);
		return comp1 < comp2;
	}

	static bool SortByFolder(const MudiffFun& mufun1, const MudiffFun& mufun2)
	{
		std::string comp1 = mufun1._folder, comp2 = mufun2._folder;
		std::transform(comp1.begin(), comp1.end(), comp1.begin(), ::tolower);
		std::transform(comp2.begin(), comp2.end(), comp2.begin(), ::tolower);
		return comp1 < comp2;
	}


	static bool SortById(const MudiffFun& mufun1, const MudiffFun& mufun2)
	{
		return mufun1._id < mufun2._id;
	}

};
int MudiffFun::Nb_of_Fun = 0;

std::ostream& operator<<(std::ostream& out, const MudiffFun& Fun){
	return out << "---------------\nId of function: " << Fun._id << "\nName: " << Fun._name << "\nFolder: " << Fun._folder << "\nSummary: " << Fun._summary << std::endl;
}

std::string LatexMaker(std::string str)
{
  std::string res = "";
  int strsize = str.size();
  int cpt_str = 0;
  while (cpt_str < strsize)
    {
      if(str[cpt_str] == '_')
	{
	res += '\\';
	res += str[cpt_str];
	cpt_str++;
	}
      else if(str[cpt_str] == '.' && cpt_str+1<strsize)
	{
	  if(str[cpt_str+1] == '.' && cpt_str + 2<strsize)
	    {
	      if(str[cpt_str+2] == '.')
	      {
		res+="\\ldots";
		cpt_str+=3;
	      }
	    }
	  else // It's only a dot or 2 dots...
	    {
	      res += str[cpt_str];
	      cpt_str++;
	    }
	}
      else // Nothing wrong
	{
	res += str[cpt_str];
	cpt_str++;
	}
    }
  return res;
}

// Delete "_" in a string
std::string LatexDeleteUnderscore(std::string str)
{
  std::string res = "";
  int strsize = str.size();
  int cpt_str = 0;
  while (cpt_str < strsize)
    {
      if(str[cpt_str] == '_')
	cpt_str++;
      else // Nothing wrong
	{
	res += str[cpt_str];
	cpt_str++;
	}
    }
  return res;
}

//Delete "_" and the ".m" in the end of a file (WARNING : NO CHECK)
std::string LatexDeleteUnderscoreAndDot(std::string str)
{
  std::string res_aux = LatexDeleteUnderscore(str);
  std::string res = res_aux.substr(0, res_aux.size()-2);
  return res;

}


//Return what behind <baliseName>...</baliseName>
std::string ParseXML(std::string baliseName, std::string rootString)
{
	std::string res = "";
	std::string balise_start, balise_end;
	balise_start = "<" + baliseName + ">";
	balise_end = "</" + baliseName + ">";

	std::size_t start = rootString.find(balise_start) + balise_start.size();
	std::size_t end = rootString.find(balise_end);

	return rootString.substr(start, end-start);

}


int main(int argc, char* argv[]) {
	std::vector<MudiffFun> MyFunctions;
	MyFunctions.resize(500);
	int id = 0;

	std::ifstream myFile("listoffuns.xml");
	std::string tline;
	bool newfun = false;
	std::string current_values;
	while (getline(myFile, tline))
	{
		std::size_t foundStart = tline.find("<fun>");
		std::size_t foundEnd = tline.find("</fun>");
		if (foundStart != std::string::npos && foundEnd != std::string::npos)
		{
			std::cout << "Big problem Dude ! <fun> and </fun> in the same line... Skipped !";
		}
		else if (foundStart != std::string::npos)
		{
			if (newfun)
				std::cout << "Problem: unfinished </fun>: skipped" << std::endl;
			newfun = true;
			current_values = "";
		}
		else if (foundEnd != std::string::npos)
		{
			newfun = false;
			//extract values
			MyFunctions[id]._name = ParseXML("name", current_values);
			MyFunctions[id]._folder = ParseXML("folder", current_values);
			MyFunctions[id]._summary = ParseXML("summary", current_values);
			id++;
		}
		if (newfun)
			current_values += tline;

	}
	myFile.close();
	int nfun = id;
	MyFunctions.resize(nfun);
//	for (int i = 0; i < id; i++)
	//	std::cout << MyFunctions[i];
	std::cout << "Num of functions: " << nfun << std::endl;

	//Write latex tabular
	std::ofstream LatexFromName("TableFromName.tex");
	std::sort(MyFunctions.begin(), MyFunctions.end(), MudiffFun::SortByName);
	for (int i = 0; i < nfun; i++)
		std::cout << MyFunctions[i];
	// Il ne reste plus qu'à écrire le tabular latex :) (pair ou impair pour la couleur...)
	for (int cpt = 0; cpt < nfun; cpt++)
	  if(cpt % 2 == 0)
	    LatexFromName << "\\funByNameEven{\\"<< LatexDeleteUnderscoreAndDot(MyFunctions[cpt]._name) << "}{" << LatexMaker(MyFunctions[cpt]._folder) << "}{" << LatexMaker(MyFunctions[cpt]._summary) << "}"<<std::endl;
	  else
	    LatexFromName << "\\funByNameUneven{\\"<< LatexDeleteUnderscoreAndDot(MyFunctions[cpt]._name) << "}{" << LatexMaker(MyFunctions[cpt]._folder) << "}{" << LatexMaker(MyFunctions[cpt]._summary) << "}"<<std::endl;

	LatexFromName.close();

	// From folder
	std::ofstream LatexFromFolder("TableFromFolder.tex");
	std::sort(MyFunctions.begin(), MyFunctions.end(), MudiffFun::SortByFolder);
	std::string current_folder= "";
	for (int cpt = 0; cpt < nfun; cpt++)
	  {
	    if(current_folder.compare(MyFunctions[cpt]._folder) !=0)
	      {
		//New section
		LatexFromFolder << "\\NewFolder{"<< LatexMaker(MyFunctions[cpt]._folder)<< "}"<< std::endl;
		current_folder = MyFunctions[cpt]._folder;
	      }
	    if(cpt % 2 ==0)
	      LatexFromFolder << "\\funByFolderEven{\\"<< LatexDeleteUnderscoreAndDot(MyFunctions[cpt]._name) << "}{"<< LatexMaker(MyFunctions[cpt]._summary) << "}"<<std::endl;
	    else
	      LatexFromFolder << "\\funByFolderUneven{\\"<< LatexDeleteUnderscoreAndDot(MyFunctions[cpt]._name) << "}{"<< LatexMaker(MyFunctions[cpt]._summary) << "}"<<std::endl;

	  }
	LatexFromFolder.close();

	// Style sheet LaTeX
	std::ofstream LatexSTY("mudiff.sty");
	std::sort(MyFunctions.begin(), MyFunctions.end(), MudiffFun::SortByName);
	for (int cpt = 0; cpt < nfun; cpt++)
	  {
	    std::string str_aux = (MyFunctions[cpt]._name);
	    std::string str = str_aux.substr(0, str_aux.size()-2);
		LatexSTY << "\\newcommand{\\" << LatexDeleteUnderscore(str) <<"}{\\hyperref[secFun:" << LatexDeleteUnderscore(str) << "]{\\code{" << LatexMaker(str) << "}}\\xspace}" << std::endl;
	  }
	LatexSTY.close();
	

}
