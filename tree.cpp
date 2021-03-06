/*
 * VesselTree - MRI image segmentation and characterization
 * 
 * Copyright 2014  Piotr M. Szczypiński <piotr.szczypinski@p.lodz.pl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tree.h"

//-------------------------------------------------------------------------------------
Tree::Tree(double mindistance)
{
    joinDistance = mindistance;
}

//-------------------------------------------------------------------------------------
bool Tree::save(const char *fileName, unsigned int format)
{
    std::ofstream file;
    file.open(fileName);
    if (!file.is_open()) return false;
    if (!file.good()) return false;

    switch(format)
    {
    case 0:
    {
        file << "@TreeSkeleton2014_Internal"<< std::endl;
        file << "@NumberOfAllNodes " << nodes.size() << std::endl;
        for(std::vector<NodeIn3D>::iterator n = nodes.begin(); n != nodes.end(); ++n)
        {
            file << "\t" << n->x << " " << n->y << " " << n->z << " " << n->connections << " "<< n->diameter << std::endl;
        }
        file << "@NumberOfBranches " << branches.size() << std::endl;
        for(std::vector<BasicBranch>::iterator b = branches.begin(); b != branches.end(); ++b)
        {
            file << "\t" << b->nodeIndex.size();
            for(std::vector<unsigned int>::iterator n = b->nodeIndex.begin(); n != b->nodeIndex.end(); ++n)
            {
                file << " " << *n;
            }
            file << std::endl;
        }
    } break;

    case 1:
    {
        file << "@TreeSkeleton2014_Simple"<< std::endl;
        file << "@NumberOfBranches " << branches.size() << std::endl;
        for(std::vector<BasicBranch>::iterator b = branches.begin(); b != branches.end(); ++b)
        {
            file << "@NumberOfNodes " << b->nodeIndex.size() << std::endl;
            for(std::vector<unsigned int>::iterator nn = b->nodeIndex.begin(); nn != b->nodeIndex.end(); ++nn)
            {
                NodeIn3D* n = &(nodes[*nn]);
                file << "\t" << n->x << " " << n->y << " " << n->z << " " << n->connections << " "<< n->diameter << std::endl;
            }
        }
    } break;
    }
    file.close();
    return true;
    
}

//-------------------------------------------------------------------------------------
bool Tree::load(const char *fileName)
{
    std::ifstream file;
    file.open(fileName);
    if (!file.is_open()) return false;
    if (!file.good()) return false;

    Tree newtree;
    std::string inputstring;
    file >> inputstring;

    if(inputstring == "@TreeSkeleton2014_Internal")
    {
        int NumberOfAllNodes;
        file>>inputstring; if(inputstring != "@NumberOfAllNodes") return false;
        file>>NumberOfAllNodes; if(NumberOfAllNodes <= 0) return false;
        for(int n = 0; n < NumberOfAllNodes; n++)
        {
            NodeIn3D newnode;
            file >> newnode.x >> newnode.y >> newnode.z >> newnode.connections >> newnode.diameter;
            newtree.nodes.push_back(newnode);
        }

        int NumberOfBranches;
        file>>inputstring; if(inputstring != "@NumberOfBranches") return false;
        file>>NumberOfBranches; if(NumberOfBranches <= 0) return false;
        for(int b = 0; b < NumberOfBranches; b++)
        {
            BasicBranch newbranch;
            int NumberOfNodes;
            file>>NumberOfNodes; if(NumberOfNodes <= 0) return false;
            for(int n = 0; n < NumberOfNodes; n++)
            {
                unsigned int newindex;
                file>>newindex; if(newindex <= 0) return false;
                newbranch.nodeIndex.push_back(newindex);
            }
            newtree.branches.push_back(newbranch);
        }
    }
    else if(inputstring == "@TreeSkeleton2014_Simple")
    {
        int NumberOfBranches;
        file>>inputstring; if(inputstring != "@NumberOfBranches") return false;
        file>>NumberOfBranches; if(NumberOfBranches <= 0) return false;
        for(int b = 0; b < NumberOfBranches; b++)
        {
            std::vector<NodeIn3D> newbranch;
            int NumberOfNodes;
            file>>inputstring; if(inputstring != "@NumberOfNodes") return false;
            file>>NumberOfNodes; if(NumberOfNodes <= 0) return false;
            for(int n = 0; n < NumberOfNodes; n++)
            {
                NodeIn3D newnode;
                file >> newnode.x >> newnode.y >> newnode.z >> newnode.connections >> newnode.diameter;
                newbranch.push_back(newnode);
            }
            newtree.addBranch(newbranch);
        }
    }
    else return false;

    nodes = newtree.nodes;
    branches = newtree.branches;
    file.close();
    return true;
}

//-------------------------------------------------------------------------------------
bool Tree::addBranch(std::vector<NodeIn3D> branch)
{
    int n;
    int bsize = branch.size();
    std::vector<unsigned int> newindex;
    newindex.resize(bsize);

    unsigned int next = nodes.size();

    for(n = 1; n < bsize-1; n++)
    {
        branch[n].connections = 2;
    }
    branch[0].connections = 1;
    branch[bsize-1].connections = 1;

// Verify new indexes
    for(n = 0; n < nodes.size(); n++)
    {
        if((fabs(branch[0].x - nodes[n].x) <= joinDistance) &&
           (fabs(branch[0].y - nodes[n].y) <= joinDistance) &&
           (fabs(branch[0].z - nodes[n].z) <= joinDistance))
        {
            newindex[0] = n;
            branch[0].connections += nodes[n].connections;
            break;
        }
    }
    if(n >= nodes.size())
    {
        newindex[0] = next;
        next++;
    }
    for(n = 1; n < bsize-1; n++)
    {
        newindex[n] = next;
        next++;
    }
    for(n = 0; n < nodes.size(); n++)
    {
        if((fabs(branch[bsize-1].x - nodes[n].x) <= joinDistance) &&
           (fabs(branch[bsize-1].y - nodes[n].y) <= joinDistance) &&
           (fabs(branch[bsize-1].z - nodes[n].z) <= joinDistance))
        {
            newindex[bsize-1] = n;
            branch[bsize-1].connections += nodes[n].connections;
            break;
        }
    }
    if(n >= nodes.size())
    {
        newindex[bsize-1] = next;
        next++;
    }

// Add or refresh nodes props
    nodes.resize(next);
    for(n = 0; n < bsize; n++)
    {
        nodes[newindex[n]] = branch[n];
    }

// Add branch
    BasicBranch newBranchI;
    newBranchI.nodeIndex = newindex;
    branches.push_back(newBranchI);

// Cut branches
    bsize = branches.size();
    for(int b = 0; b < bsize; b++)
    {
        int bs = branches[b].nodeIndex.size();
        for(n = 1; n < bs-1; n++)
        {
            if(nodes[ branches[b].nodeIndex[n] ].connections > 2) break;
        }
        if(n < bs-1)
        {
            int i;
            BasicBranch b1;
            BasicBranch b2;

            for(i = 0; i <= n; i++) b1.nodeIndex.push_back(branches[b].nodeIndex[i]);
            for(i = n; i < bs; i++) b2.nodeIndex.push_back(branches[b].nodeIndex[i]);
            branches[b] = b1;
            branches.push_back(b2);
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------
bool Tree::removeBranch(unsigned int ib)
{
    int n;
    int bsize = branches.size();
    if(ib >= (unsigned int)bsize) return false;

    int nsize = branches[ib].nodeIndex.size();

//Indicate empty nodes
    for(n = 1; n < nsize-1; n++)
    {
        nodes[branches[ib].nodeIndex[n]].connections = 0;
    }
    nodes[branches[ib].nodeIndex[0]].connections--;
    nodes[branches[ib].nodeIndex[nsize-1]].connections--;

//Remove branch
    std::vector<BasicBranch>::iterator itb = branches.begin()+ib;
    branches.erase(itb);

//Remove empty nodes and reindex branch entries
    for(n = nodes.size()-1; n >= 0; n--)
    {
        if(nodes[n].connections == 0)
        {
            std::vector<NodeIn3D>::iterator it = nodes.begin()+n;
            nodes.erase(it);

            for(std::vector<BasicBranch>::iterator b = branches.begin(); b != branches.end(); ++b)
            {
                for(std::vector<unsigned int>::iterator nn = b->nodeIndex.begin(); nn != b->nodeIndex.end(); ++nn)
                {
                    if(*nn > n) (*nn)--;
                }
            }
        }
    }
//  Merge branches
    bsize = branches.size();
    for(int b = 0; b < bsize; b++)
    {
        int bs = branches[b].nodeIndex.size();
        if(bs <= 0) continue;

        int index = -1;
        bool at0 = true;
        if(nodes[ branches[b].nodeIndex[0] ].connections == 2) index = branches[b].nodeIndex[0];
        else if(nodes[ branches[b].nodeIndex[bs-1] ].connections == 2)
        {
            index = branches[b].nodeIndex[bs-1];
            at0 = false;
        }

        if(index >= 0)
        {
            for(int p = b+1; p < bsize; p++)
            {
                int i;
                int ps = branches[p].nodeIndex.size();
                if(ps <= 0) continue;
                BasicBranch nbranch;

                if(branches[p].nodeIndex[0] == index && at0)
                {
                    if(at0)
                    {
                        for(i = ps-1; i > 0; i--) nbranch.nodeIndex.push_back(branches[p].nodeIndex[i]);
                        for(i = 0; i < bs; i++) nbranch.nodeIndex.push_back(branches[b].nodeIndex[i]);
                    }
                    else
                    {
                        for(i = 0; i < bs; i++) nbranch.nodeIndex.push_back(branches[b].nodeIndex[i]);
                        for(i = 1; i < ps; i++) nbranch.nodeIndex.push_back(branches[p].nodeIndex[i]);
                    }
                    std::vector<BasicBranch>::iterator ib = branches.begin()+p;
                    branches.erase(ib);
                    ib = branches.begin()+b;
                    branches.erase(ib);
                    branches.push_back(nbranch);
                    return true;
                }
                if(branches[p].nodeIndex[ps-1] == index)
                {
                    if(at0)
                    {
                        for(i = 0; i < ps; i++) nbranch.nodeIndex.push_back(branches[p].nodeIndex[i]);
                        for(i = 1; i < bs; i++) nbranch.nodeIndex.push_back(branches[b].nodeIndex[i]);
                    }
                    else
                    {
                        for(i = 0; i < bs; i++) nbranch.nodeIndex.push_back(branches[b].nodeIndex[i]);
                        for(i = ps-2; i >= 0; i--) nbranch.nodeIndex.push_back(branches[p].nodeIndex[i]);
                    }
                    std::vector<BasicBranch>::iterator ib = branches.begin()+p;
                    branches.erase(ib);
                    ib = branches.begin()+b;
                    branches.erase(ib);
                    branches.push_back(nbranch);
                    return true;
                }
            }
        }
    }
    return true;
}


//-------------------------------------------------------------------------------------
unsigned int Tree::nodeCount(void)
{
    return nodes.size();
}


//-------------------------------------------------------------------------------------
unsigned int Tree::count(void)
{
    return branches.size();
}

//-------------------------------------------------------------------------------------
int Tree::count(unsigned int ib)
{
    if(ib >= branches.size()) return -1;
    return branches[ib].nodeIndex.size();
}

//-------------------------------------------------------------------------------------
std::vector<NodeIn3D> Tree::branch(unsigned int ib)
{
    std::vector<NodeIn3D> r;
    if(ib >= branches.size()) return r;

    for(std::vector<unsigned int>::iterator nn = branches[ib].nodeIndex.begin(); nn != branches[ib].nodeIndex.end(); ++nn)
    {
        r.push_back(nodes[*nn]);
    }
    return r;
}

//-------------------------------------------------------------------------------------
NodeIn3D Tree::node(unsigned int ib, unsigned int in)
{
    NodeIn3D r;
    if(ib >= branches.size()) return r;
    if(in >= branches[ib].nodeIndex.size()) return r;
    if(branches[ib].nodeIndex[in] >= nodes.size()) return r;
    r = nodes[branches[ib].nodeIndex[in]];
    return r;
}

//-------------------------------------------------------------------------------------
NodeIn3D Tree::node(unsigned int i)
{
    NodeIn3D r;
    if(i >= nodes.size()) return r;
    r = nodes[i];
    return r;
}

//-------------------------------------------------------------------------------------
bool Tree::setNode(NodeIn3D node, unsigned int i)
{
    if(i >= nodes.size()) return false;
    nodes[i] = node;
    return true;
}

/*
bool Tree::addPoint(double x, double y, double z, unsigned int con, unsigned int dia)
{
    NodeIn3D input;
    input.x = x;
    input.y = y;
    input.z = z;
    input.connections = con;
    input.diameter = dia;
    nodes.push_back(input);
    return true;
}
*/
