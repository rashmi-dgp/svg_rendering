#include "mesh_factory.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "nanosvg.h"
#include "config.h"
using namespace std;
void GenerateCubicBezierVertices(const Vertex& startPoint, const Vertex& controlPoint1, const Vertex& controlPoint2, const Vertex& endPoint, int numLines, std::vector<Vertex> &vertices, std::vector<ushort> &indices, ushort &index){
    for(float i = 0; i<=1; i+=0.002){
        float t = 1.0f*i;
        float t2 = t*t;
        float t3 = t2*t;
        float t_dash = (1.0f - t);
        float t_dash2 = t_dash*t_dash;
        float t_dash3 = t_dash*t_dash2;

        Vertex cur;
        cur.pos[0] = (t_dash3) * startPoint.pos[0] + (3*t_dash2*t)*controlPoint1
            .pos[0] + (3*t_dash*t2)*controlPoint2.pos[0] + t3*endPoint.pos[0];
        cur.pos[1] = (t_dash3) * startPoint.pos[1] + (3*t_dash2*t)*controlPoint1.pos[1] + (3*t_dash*t2)*controlPoint2.pos[1] + t3*endPoint.pos[1];


        std::clamp(cur.pos[0], -1.0f, 1.0f);
        std::clamp(cur.pos[1], -1.0f, 1.0f);
        cur.color= {0.0f,0.0f,0.0f};
        vertices.push_back(cur);
//        indices.push_back(index);
//        index++;
    }
}
std::vector<Vertex> GenerateCubicBezierVerticesFromPoints( const Vertex& p0, const Vertex& p1, const Vertex& p2, const Vertex& p3, int numLines){
    std::vector<Vertex> vertices;
    for(int i = 0; i<=numLines; i++){
        // generate the vertex for the current value of t
        float t = 1.0f*i/numLines;
        float t2 = t*t;
        float t3 = t2*t;
        float t_dash = (1.0f - t);
        float t_dash2 = t_dash*t_dash;
        float t_dash3 = t_dash*t_dash;
        Vertex cur;
        cur.pos[0] = (t_dash3) * p0.pos[0] + (3*t_dash2*t)*p1.pos[0] + (3*t_dash*t2)*p2.pos[0] + t3*p3.pos[0];
        cur.pos[1] = (t_dash3) * p0.pos[1] + (3*t_dash2*t)*p1.pos[1] + (3*t_dash*t2)*p2.pos[1] + t3*p3.pos[1];
        std::clamp(cur.pos[0], -1.0f, 1.0f);
        std::clamp(cur.pos[1], -1.0f, 1.0f);
        vertices.push_back(cur);
//        return cur;
    }
    return vertices;
}
void GenerateCBFromBezeir(const std::vector<Vertex>& vertices, std::vector<Vertex>& newVertices) {
    int noOfGroups = vertices.size() / 4;
    for (int i = 0; i < noOfGroups; ++i) {
        std::vector<Vertex> bezierVertices = GenerateCubicBezierVerticesFromPoints(
            vertices[i * 4], vertices[i * 4 + 1], vertices[i * 4 + 2], vertices[i * 4 + 3], 100); // Assuming 100 lines
        newVertices.insert(newVertices.end(), bezierVertices.begin(), bezierVertices.end());
    }
}
MTL::Buffer* MeshFactory::buildTriangle(MTL::Device* device) {
    // Declare the data to send
    Vertex vertices[3] = {
        {{-0.75, -0.75}, {1.0, 1.0, 0.0}},
        {{ 0.75, -0.75}, {1.0, 1.0, 0.0}},
        {{  0.0,  0.75}, {1.0, 1.0, 0.0}}
    };
    // Create a buffer to hold it
    MTL::Buffer* buffer = device->newBuffer(3 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    // Upload to buffer
    memcpy(buffer->contents(), vertices, 3 * sizeof(Vertex));
    return buffer;
}
Mesh MeshFactory::buildQuad(MTL::Device* device) {
    Mesh mesh;
    // Declare the data to send
    Vertex vertices[4] = {
        {{-0.75, -0.75}, {0.0, 0.0, 0.0}},
        {{ 0.75, -0.75}, {0.0, 0.0, 0.0}},
        {{ 0.75,  0.75}, {0.0, 0.0, 0.0}},
        {{-0.75,  0.75}, {0.0, 0.0, 0.0}},
    };
    ushort indices[] = {
        0, 1,  // Bottom line
        1, 2,  // Right line
        2, 3,  // Top line
        3, 0   // Left line
    };
    // Vertex buffer
    mesh.vertexBuffer = device->newBuffer(4 * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices, 4 * sizeof(Vertex));

    // Index buffer
    mesh.indexBuffer = device->newBuffer(8 * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices, 8 * sizeof(ushort));

    return mesh;
}
Mesh MeshFactory::buildSVG(MTL::Device* device, const char* svgFilePath) {
    Mesh mesh;
    std::vector<Vertex> vertices;
    std::vector<ushort> indices;
    // Load SVG
    NSVGimage* image = nsvgParseFromFile(svgFilePath, "px", 96);
    if (!image) {
        std::cerr << "Could not open SVG image." << std::endl;
        return mesh;
    }
    ushort index = 0;
    std::ofstream outFile("cubic_bezier_shapes.txt");
    // go through all shapes and calculate bounds: xMax and yMax
    float xMax=0.0f, yMax=0.0f, xMin=0.0f, yMin=0.0f;
    for (NSVGshape* shape = image->shapes; shape != nullptr; shape = shape->next){
        xMin = shape->bounds[0];
        yMin = shape->bounds[1];
        xMax = shape->bounds[2];
        yMax = shape->bounds[3];
    }
    std::cout<<xMin<<" "<<xMax<<" "<<yMin<<" "<<yMax<<"\n";
    float imageWidht, imageHeight;
//    NSVGimage* nsvgImage;
    float widthOfImage = image->width;
    float heightOfImage= image->height;
    std::cout<<widthOfImage<<" "<<heightOfImage<<"\n";
    for (NSVGshape* shape = image->shapes; shape != nullptr; shape = shape->next){
        for (NSVGpath* path = shape->paths; path != nullptr; path = path->next){
            for (int i = 0; i < path->npts - 1; i += 3){
                float* p = &path->pts[i * 2];
                outFile << "Shape ID: " << (shape->id ? shape->id : "none") << ", Path Points:";
                std::vector<Vertex> temp;
                for (int j = 0; j < 4; ++j){
                    temp.push_back({{(p[j * 2] / 600), (p[j * 2 + 1] / 600)}, {1.0, 0.0, 0.0}});
//                    indices.push_back(index++);
                    outFile << "[" << 2*(p[j * 2] / 600) - 1.0f<< ", " << 2*(p[j * 2 + 1] / 600)-1.0f << "]";
                    if (j < 3) outFile << ", ";
                }
                GenerateCubicBezierVertices(temp[0], temp[1], temp[2], temp[3], 100, vertices, indices, index);
                outFile << std::endl;
            }
        }
    }
    mesh.vertexBuffer = device->newBuffer(vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeShared);
    memcpy(mesh.vertexBuffer->contents(), vertices.data(), vertices.size() * sizeof(Vertex));
//    int i = 0;
    for(int i =1; i< vertices.size(); i++)
    {
        indices.push_back(i-1);
        indices.push_back(i);
    }
    mesh.indexBuffer = device->newBuffer(indices.size() * sizeof(ushort), MTL::ResourceStorageModeShared);
    memcpy(mesh.indexBuffer->contents(), indices.data(), indices.size() * sizeof(ushort));
    return mesh;
   }
