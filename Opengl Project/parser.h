#include"plane_t.h"
#include <sstream>
#include <fstream>
#include<algorithm>
#include"Basic.h"

//Inefficient parser
//merge sort is use to sort on the basis of z and draw
//maybe removed after camera


//workflow
//Obj file is opened first and then
//first scan the vertices(v),vertex texture(vt)
//,material id(id) and the avg vertexnormal(vn)

//mtl file is then opened
//according to the above scaned material id scan the 
// properties of material and add to vector even if its 
//already read before it is read again if the material is reused


//now again obj file is reopened and then which material 
// the face is using is scanned and then according to the 
// indices scanned plane is created and pushed to the vector

//parser to load obj files already divided into triangles
namespace parser
{
        void managetex(coordinate2f& vt)
        {
            double intpart;
            if (vt.x > 1)
            {
                vt.x = modf(vt.x, &intpart);
                if (int(intpart * 10) == 0)
                    vt.x = 1;
            }
            if (vt.y > 1)
            {
                vt.y = modf(vt.x, &intpart);
                if (int(intpart * 10) == 0)
                    vt.y = 1;
            }
        }

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

        std::vector<plane_t> parse(std::string filename,std::vector<texture> &tex,coordinate3f &scalefactor,std::vector<coordinate3f> &vertexes)
        {
            std::vector<plane_t> planes;
            std::vector<coordinate3f> textures;
            std::vector<coordinate3f> normal;
            std::vector<material> materials;
            std::string data, lineStr;
            std::fstream obj(filename + ".obj", std::ios::in);
            if (!obj)
            {
                std::cout << filename << " opening error" << std::endl;
                return planes;
            }
            
            std::istringstream lineSS;
            std::vector<std::string> materialids;
            std::string lineType;
            std::vector<float> normalizev[3];
            std::vector<float> normalizevt[2];
            while (std::getline(obj, lineStr))
            {
                lineSS = std::istringstream(lineStr);;
                lineSS >> lineType;
                if (lineType == "v")
                {
                    float x = 0, y = 0, z = 0;
                    lineSS >> x >> y >> z;
                    vertexes.push_back(coordinate3f(x, y, z));
                    normalizev[0].push_back(x);
                    normalizev[1].push_back(y);
                    normalizev[2].push_back(z);
                }
                else if (lineType == "vt")
                {
                    float u = 0, v = 0;
                    lineSS >> u >> v;
                    textures.push_back(coordinate3f(u, v,1));
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
            std::sort(normalizev[0].begin(), normalizev[0].end());
            std::sort(normalizev[1].begin(), normalizev[1].end());
            std::sort(normalizev[2].begin(), normalizev[2].end());
            

            
            coordinate3f scale(
                normalizev[0].back() - normalizev[0].front(),
                normalizev[1].back() - normalizev[1].front(),
                normalizev[2].back() - normalizev[2].front()
            );
            scale = coordinate3f(1/scale.x, 1/scale.y, 1/scale.y);
            //translate (x,y,z) by (-xmin,-ymin,zmin)
            for (int i = 0; i < vertexes.size(); i++)
            {
                vertexes[i] = vertexes[i] + coordinate3f(-normalizev[0][0], -normalizev[1][0], normalizev[2][0]);
                vertexes[i] = vertexes[i].scaling(scale.x,scale.y,scale.z);
            }
            int  count=0;
            for (auto i : textures)
            {
                if (i.x > 1 || i.y > 1)
                    count++;
            }
           
            float ratioyx = scale.x/scale.y;
            float ratiozx = scale.x / scale.z;

            std::fstream mtl(filename + ".mtl", std::ios::in);
            for (auto i : materialids)
            {
                bool found=0;
                while (std::getline(mtl, lineStr))
                {
                    if (lineStr == "newmtl " + i)
                    {
                        material temp;
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
                        
                        getline(mtl, lineStr);
                        getline(mtl, lineStr);
                        lineSS = std::istringstream(lineStr);
                        lineSS >> lineType;

                        
                        if (lineType == "map_Kd")
                        {
                            std::string texfilename,copy;
                            lineSS >> texfilename;

                            while (lineSS>>copy)
                            {
                                texfilename += " " + copy;
                            }

                            for (int i = 0; i < tex.size(); i++)
                            {
                                if (tex[i].filename == texfilename)
                                {
                                    temp.tex = &tex[i];
                                }
                            }

                            if (!temp.tex)
                            {
                                texture _t(texfilename);
                                if (_t.load())
                                {
                                    tex.push_back(_t);
                                    temp.tex = &tex[tex.size() - 1];
                                }
                            }
                        }
                        found = 1;
                        materials.push_back(temp);
                        break;
                    }
                }

                if (!found)
                    materials.push_back(material());

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
                    if(textures.size() != 0 && sep[1][0] != -1)
                    {
                        p.vt = {
                                   textures[sep[1][0]],
                                   textures[sep[1][1]],
                                   textures[sep[1][2]]
                        };
                    }

                    float fac = SCREEN_WIDTH;

                    p.scale(fac, ratioyx* fac, ratiozx* fac);
                    
                    scalefactor.x = fac;
                    scalefactor.y=ratioyx* fac;
                    scalefactor.z=ratiozx* fac;

                    plane_t t(p, materialBind);
                    t.makeCalculations();
                    planes.push_back(t);
                 }
            }
            std::cout << "Triangles Recorded" << planes.size()<<std::endl;
            std::cout << "Vertices Recorded" << vertexes.size()<<std::endl;
            return planes;
        }
};