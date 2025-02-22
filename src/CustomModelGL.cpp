#include "CustomModelGL.h"


CustomModelGL::CustomModelGL(std::string name, int _nbElements){
    this->m_Name = name;

    m_nbElements = _nbElements;

    m_Model = new GeometricModel();

    createDeformableGrid();
}


void CustomModelGL::createDeformableGrid()
{
	for (float i = 0; i < m_nbElements; i++)
		for (float j = 0; j < m_nbElements; j++)
		{
			
			if (i < m_nbElements - 1 && j < m_nbElements - 1)
			{
				Face f;
				f.s3 = (int)m_Model->listVertex.size();
				f.s2 = (int)m_Model->listVertex.size() + 1;
				f.s1 = (int)m_Model->listVertex.size() + 1 + m_nbElements;
				m_Model->listFaces.push_back(f);
				m_Model->listCoordFaces.push_back(f);

				f.s3 = (int)m_Model->listVertex.size();
				f.s2 = (int)m_Model->listVertex.size() + 1 + m_nbElements;
				f.s1 = (int)m_Model->listVertex.size() + m_nbElements;
				m_Model->listFaces.push_back(f);
				m_Model->listCoordFaces.push_back(f);


			}
			m_Model->listVertex.push_back(glm::vec3(i, j, 0));
			m_Model->listCoords.push_back(glm::vec3((float)i/(float)m_nbElements, (float)j/(float)m_nbElements, 0));
			V.push_back(glm::vec3(0.0f));
			F.push_back(glm::vec3(0.0f));

		}
	//Resize and center

	glm::vec3 mean = glm::vec3(0.0f);
	glm::vec4 size = glm::vec4(0.0f);

	for (glm::vec3 v : m_Model->listVertex)
	{
		mean += v;
		size.x = std::min(v.x, size.x);
		size.y = std::max(v.x, size.y);

		size.z = std::min(v.y, size.z);
		size.w = std::max(v.y, size.w);
	}
	mean /= (float)m_Model->listVertex.size();
	for (int i = 0; i < (int)m_Model->listVertex.size(); i++)
	{
		m_Model->listVertex[i] = (m_Model->listVertex[i] - mean);
		m_Model->listVertex[i].x /= (size.y - size.x);
		m_Model->listVertex[i].y /= (size.w - size.z);
	}

    m_Model->nb_vertex = (int)m_Model->listVertex.size();
    m_Model->nb_faces = (int)m_Model->listFaces.size();
    m_Model->loader->computeNormalAndTangents(m_Model);

    /**************A COMPLETER *********************************************************
    * Ajouter ici des ressorts entre les �l�ments de la grille 2D composant notre objet d�formable. Chaque ressort est une structure spring a rentrer dans le tableau springs.
    * struct Spring
    {
	    int id1;	// Indice du sommet 1
	    int id2;	// Indice du sommet 2
	    float length; // longueur au repos. D�finir la longueur initiale entre les 2 sommets par exemple
	    float KsFactor; // facteur de rigidit�. // modulation du facteur de rigidit� global. Ce facteur va "moduler" le facteur de rigidit� globale pass� en param�tre. Utiliser 1.0 par exemple pour un ressort standard, 0.75 ou 0.5, ou moins pour un ressort plus fainle.
    };

    Il est conseill� de d�finir au moins un ressort standard entre chaque paire d'�l�ments de la grille en voisinage direct (haut,bas, gauche ,droite).
    Il peut �tre �galement utile de d�finir des ressorts entre les �l�ments diagnouax , voir de voisinage plus eloign� (sommets a 2 el�ments de distance). Exp�rimentez diff�rentes strat�gies et mod�les afin d'obtenir un r�sultat convaincant.

    */

	float springLength = glm::length(m_Model->listVertex[0] - m_Model->listVertex[1]);
    float springDiagLength = glm::sqrt(springLength * springLength + springLength * springLength);
	float KsFactor = 1.0f;

    // Ressorts pour chaques voisins direct (manhattan)

    for (int row = 0; row < m_nbElements; row++)
    {
        for (int column = 0; column < m_nbElements; column++)
        {
            if (row > 0)
            {
	            int north = (row - 1) * m_nbElements + column;
            	AddSpring(indice(row, column), north, springLength, KsFactor);
            }
        	if (row < m_nbElements - 1)
        	{
        		int south = (row + 1) * m_nbElements + column;
        		AddSpring(indice(row, column), south, springLength, KsFactor);
        	}
        	if (column > 0)
        	{
        		int west = row * m_nbElements + (column - 1);
        		AddSpring(indice(row, column), west, springLength, KsFactor);
        	}
        	if (column < m_nbElements - 1)
        	{
        		int east = row * m_nbElements + (column + 1);
        		AddSpring(indice(row, column), east, springLength, KsFactor);
        	}
        }
    }

	// Ressorts pour chaques voisins 2 straight steps away

	for (int row = 0; row < m_nbElements; row++)
	{
		for (int column = 0; column < m_nbElements; column++)
		{
			if (row >= 2)
			{
				int north = (row - 2) * m_nbElements + column;
				AddSpring(indice(row, column), north, springLength * 2.0f, KsFactor / 2.0f);
			}
			if (row < m_nbElements - 2)
			{
				int south = (row + 2) * m_nbElements + column;
                AddSpring(indice(row, column), south, springLength * 2.0f, KsFactor / 2.0f);
			}
			if (column >= 2)
			{
				int west = row * m_nbElements + (column - 2);
                AddSpring(indice(row, column), west, springLength * 2.0f, KsFactor / 2.0f);
			}
			if (column < m_nbElements - 2)
			{
				int east = row * m_nbElements + (column + 2);
                AddSpring(indice(row, column), east, springLength * 2.0f, KsFactor / 2.0f);
			}
		}
	}

	// Ressorts pour chaques voisins diagonaux

	for (int row = 0; row < m_nbElements; row++)
	{
		for (int column = 0; column < m_nbElements; column++)
		{
			if (row > 0 && column < m_nbElements - 1)
			{
				int northEast = (row - 1) * m_nbElements + (column + 1);
				AddSpring(indice(row, column), northEast, springDiagLength, KsFactor);
			}
			if (row > 0 && column > 0)
			{
				int northWest = (row - 1) * m_nbElements + (column - 1);
				AddSpring(indice(row, column), northWest, springDiagLength, KsFactor);
			}
			if (row < m_nbElements - 1 && column < m_nbElements - 1)
			{
				int southEast = (row + 1) * m_nbElements + (column + 1);
				AddSpring(indice(row, column), southEast, springDiagLength, KsFactor);
			}
			if (row < m_nbElements - 1 && column > 0)
			{
				int southWest = (row + 1) * m_nbElements + (column - 1);
				AddSpring(indice(row, column), southWest, springDiagLength, KsFactor);
			}
		}
	}

    /********************************************************************************************************************************************/

    loadToGPU();
}

void CustomModelGL::AddSpring(int id1, int id2, float length, float KsFactor)
{
	Spring s = Spring();
	s.id1 = id1;
	s.id2 = id2;
	s.length = length;
	s.KsFactor = KsFactor;

	springs.push_back(s);
}

int CustomModelGL::indice(int i, int j)
{
	return i * m_nbElements + j;
}

void CustomModelGL::recomputeNormals()
{
	m_Model->listNormals.clear();
	m_Model->loader->computeNormals(m_Model);
}



void CustomModelGL::updatePositions()
{
	glNamedBufferData(VBO_Vertex, m_Model->nb_vertex * sizeof(glm::vec3), &(m_Model->listVertex.front()), GL_DYNAMIC_DRAW);
	glNamedBufferData(VBO_Normals, m_Model->nb_vertex * sizeof(glm::vec3), &(m_Model->listNormals.front()), GL_DYNAMIC_DRAW);
}