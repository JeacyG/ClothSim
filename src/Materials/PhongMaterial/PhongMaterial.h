

#ifndef _PhongMaterial_
#define _PhongMaterial_


#include "MaterialGL.h"




struct Phong {
	glm::vec4 coeff;
	glm::vec3 albedo;
	glm::vec3 specularColor;
};




class PhongMaterial : public MaterialGL
{
public:

	//Attributs

	//Constructeur-Destructeur

	/**
		Constructeur de la classe � partir du nom du mat�riau
		@param name : nom du mat�riau
	*/
	PhongMaterial(string name = "");

	/**
		Destructeur de la classe
	*/
	~PhongMaterial();

	//M�thodes

	/**
		M�thode virtuelle qui est appel�e pour faire le rendu d'un objet en utilisant ce mat�riau
		@param o : Node/Objet pour lequel on veut effectuer le rendu
	*/
	virtual void render(Node* o);

	/**
		M�thode virtuelle qui est appel�e pour modifier une valeur d'un param�tre n�cessaire pour le rendu
		@param o : Node/Objet concern� par le rendu
		@param elapsedTime : temps
	*/
	virtual void animate(Node* o, const float elapsedTime);



	 void updatePhong();

	virtual void displayInterface() ;


protected:
	GLProgram* vp;
	GLProgram* fp;

	GLuint l_ViewProj, l_Model, l_PosLum, l_PosCam,l_Phong,l_Albedo,l_specColor, l_Time;
	Phong param;

};

#endif