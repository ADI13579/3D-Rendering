#include"plane_t.h"
#include <sstream>
#include <fstream>

//merge sort is use to sort on the basis of z and draw
//maybe removed after camera
void merge(std::vector<plane_t>& left, std::vector<plane_t>& right, std::vector<plane_t>& bars)
{
    int nL = left.size();
    int nR = right.size();
    int i = 0, j = 0, k = 0;

    while (j < nL && k < nR)
    {
        if (left[j].centroid.z > right[k].centroid.z) {
            bars[i] = left[j];
            j++;
        }
        else {
            bars[i] = right[k];
            k++;
        }
        i++;
    }
    while (j < nL) {
        bars[i] = left[j];
        j++; i++;
    }
    while (k < nR) {
        bars[i] = right[k];
        k++; i++;
    }
}

//part of above code
void sort(std::vector<plane_t>& bar) {
    if (bar.size() <= 1) return;

    int mid = bar.size() / 2;
    std::vector<plane_t> left;
    std::vector<plane_t> right;

    for (size_t j = 0; j < mid; j++)
        left.push_back(bar[j]);
    for (size_t j = 0; j < (bar.size()) - mid; j++)
        right.push_back(bar[mid + j]);

    sort(left);
    sort(right);
    merge(left, right, bar);
}

//workflow
//Obj file is opened first and then
//first scan the vertices(v),vertex texture(vt)
//,material id(id) and the avg vertexnormal(vn)

//mtl file is then opened
//according to the above scaned material id scan the 
// properties of material and add to vector even if its 
//already read before it is read again if the material is reused


//now again obj file is reopened and then which material 
// is face using is scanned and then according to the 
// indices scanned plane is created and pushed to the vector

//parser to load object files already divided into triangles
namespace parser
{
        void transpose(std::vector<std::vector<int> >& b)
        {
            if (b.size() == 0)
                return;

            std::vector<std::vector<int>> trans_vec(b[0].size(), std::vector<int>());

            for (int i = 0; i < b.size(); i++)
            {
                for (int j = 0; j < b[i].size(); j++)
                {
                    trans_vec[j].push_back(b[i][j] - 1);
                }
            }
            b = trans_vec;
        }

        std::vector<std::vector<int>> seperate(std::istringstream& line)
        {
            std::vector<std::vector<int>> face;
            std::string t;
            while (line >> t)
            {
                std::vector<int> temp;
                std::istringstream ref(t);
                std::string vStr, vtStr, vnStr;
                std::getline(ref, vStr, '/');
                std::getline(ref, vtStr, '/');
                std::getline(ref, vnStr, '/');
                int v = atoi(vStr.c_str());
                int vt = atoi(vtStr.c_str());
                int vn = atoi(vnStr.c_str());
                temp = { v,vt,vn };
                face.push_back(temp);
            }
            transpose(face);
            return face;
        }

        std::vector<plane_t> parse(std::string filename)
        {
            std::vector<plane_t> planes;
            std::vector<coordinate3f> vertexes;
            std::vector<coordinate2f> textures;
            std::vector<coordinate3f> normal;
            std::vector<material> materials;
            std::string data, lineStr;
            std::fstream obj(filename + ".obj", std::ios::in);
            if (!obj)
            {
                std::cout << filename << " opening error" << std::endl;
                std::cout << "error opening file";
                return planes;
            }

            std::istringstream lineSS;
            std::vector<std::string> materialids;
            std::string lineType;
            while (std::getline(obj, lineStr))
            {
                lineSS = std::istringstream(lineStr);;
                lineSS >> lineType;
                if (lineType == "v")
                {
                    float x = 0, y = 0, z = 0;
                    lineSS >> x >> y >> z;
                    vertexes.push_back(coordinate3f(x, y, z));
                }
                else if (lineType == "vt")
                {
                    float u = 0, v = 0;
                    lineSS >> u >> v;
                    textures.push_back(coordinate2f(u, v));
                }
                else if (lineType == "vn")
                {
                    float i = 0, j = 0, k = 0;
                    lineSS >> i >> j >> k;
                    normal.push_back(coordinate3f(i, j, k));
                }
                else if (lineType == "usemtl")
                {
                    std::string mtlid;
                    lineSS >> mtlid;
                    materialids.push_back(mtlid);
                }
            }
            obj.close();

            
            std::fstream mtl(filename + ".mtl", std::ios::in);
            for (auto i : materialids)
            {
                material temp;
                int a = mtl.tellg();
                while (std::getline(mtl, lineStr))
                {

                    if (lineStr == "newmtl " + i)
                    {
                        temp.id = i;
                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.Ns;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.ka.x >> temp.ka.y >> temp.ka.z;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.kd.x >> temp.kd.y >> temp.kd.z;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.ks.x >> temp.ks.y >> temp.ks.z;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.ke.x >> temp.ke.y >> temp.ke.z;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.Ni.x >> temp.Ni.y >> temp.Ni.z;

                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;
                        lineSS >> temp.d;
                        materials.push_back(temp);
                        break;
                    }
                }
                mtl.clear();
                mtl.seekg(0);
            }
            mtl.close();

            int k = 0;
            material materialBind;
            obj = std::fstream(filename + ".obj", std::ios::in);
            while (std::getline(obj, lineStr))
            {
                lineSS = std::istringstream(lineStr);;
                lineSS >> lineType;
                if (lineType == "usemtl")
                {
                    materialBind = materials[k];
                    k++;
                }
                else if (lineType == "f")
                {
                    simpleplane p;
                    std::vector<std::vector<int>> sep = seperate(lineSS);

                    if (vertexes.size() != 0)
                    {
                        p.v = {
                               vertexes[sep[0][0]],
                               vertexes[sep[0][1]],
                               vertexes[sep[0][2]]
                        };
                    }
                    if (normal.size() != 0)
                    {
                        p.vn = {
                              normal[sep[2][0]],
                              normal[sep[2][1]],
                              normal[sep[2][2]],
                        };
                    }
                    if (textures.size() != 0 && sep[1][0]!=-1)
                    {
                        p.vt = {
                                   textures[sep[1][0]],
                                   textures[sep[1][1]],
                                   textures[sep[1][2]]
                        };
                    }
                    p.makeCalculations();
                    planes.push_back(plane_t(p, materialBind));
                }
            }

            sort(planes);
            
            return planes;
}
};