/**
 * Author: Kyle Keiper
 * Copyright: 2022
 * License: MIT
 *
 * Do whatever you want with this file. I don't care
 *
 * It's a single header file used for loading the HUNTDAT files from carnivores 2. Meant to be included in the style of stb files
 *
 * define the macro OCARN2_IMPLEMENTATION in **1** header file for the source to be generated
 *
 * main methods are
 *
 * Mesh load_3df_file(const char* filename);
 * Mesh load_car_file(const char* filename);
 * Rsc load_rsc_file(const char* filename);
 * Map load_map_file(const char* filename);
 */


#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace OCARN2 {
    struct Vertex {
        float x, y, z;
        short owner;
        unsigned short hidden;
    };

    struct Face {
        int v1, v2, v3;

        int tax, tbx, tcx;
        int tay, tby, tcy;

        unsigned short flags, DMask;
        int distant, next, group;
        char reserved[12];
    };

    // data is in the files, but I'm not really sure what it does
    struct Node {
        char name[32];

        float x, y, z;
        short owner;
        unsigned short hidden;
    };

    //
    struct Animation {
        char name[32];
        unsigned int kps;
        unsigned int numFrames;

        short* data;
    };

    //
    struct SoundEffect {
        char name[32];
        unsigned int length;
        unsigned short* data;
    };

    struct RandomSound {
        int number, volume, frequency;
        unsigned short environment, flags;
    };

    struct AmbientSound {
        SoundEffect sound;
        RandomSound randomSounds[16];

        int numSoundEffects;
        int volume, time;
    };

    /**
     * shared between both .car and .3df models
     * eventually share it with the terrain map geometry
     */
    struct Mesh {
        char name[24] = "Unnamed\0";
        char msc[8]{};

        unsigned int numVertices{};
        unsigned int numFaces{};
        unsigned int numNodes{};
        unsigned int textureSize{};

        unsigned int numAnimations{};
        unsigned int numSoundEffects{};

        std::vector<Face> faces;
        std::vector<Vertex> vertices;
        std::vector<Node> nodes;
        std::vector<Animation> animations;
        std::vector<SoundEffect> soundEffects;

        std::vector<int> soundMap = std::vector<int>(64);

        unsigned short* textureData{};
    };

    struct Color {
        int r, g, b, a;
    };

    struct Fog {
        int rgb;
        Color color;

        float y;
        bool isMortal;
        float transparent, fLimit;
    };

    struct Water {
        int tIndex, wLevel;
        float transparency;
        int rgb;

        Color color;
    };

    struct Texture {
        unsigned int size;
        unsigned short* data;
    };

    struct RscModel {
        int radius;
        int yLo, yHi;
        int lineLength;
        int lightIntensity;
        int circleRadius;
        int circleIntensity;

        int flags;

        int grRadius;
        int defLight;
        int lastAnimationTime;
        float boundingRadius;

        char reserved[16];

        Mesh mesh;

        int textureSize = 128 * 128 * 2;
        unsigned short* textureData;
    };

    /**
     * contains fields for all the data within a .rsc file
     */
    struct Rsc {
        unsigned int numTextures;
        unsigned int numModels;
        unsigned int numFogs;
        unsigned int numSoundEffects;
        unsigned int numAmbientSounds;
        unsigned int numWaters;

        unsigned int fadeRgb[3][3];
        unsigned int transRgb[3][3];

        std::vector<Texture> textures;
        std::vector<RscModel> models;
        std::vector<Fog> fogs;
        std::vector<SoundEffect> soundEffects;
        std::vector<AmbientSound> ambientSounds;
        std::vector<Water> waters;

        unsigned short sky[3][256 * 256];
        unsigned char skyMap[128 * 128];
    };

    /**
     *
     */
    struct Map {
        std::vector<unsigned char> heightMap = std::vector<unsigned char>(1024 * 1024);
        std::vector<unsigned short> textureMap = std::vector<unsigned short>(1024 * 1024);
        std::vector<unsigned short> textureMapFar = std::vector<unsigned short>(1024 * 1024);
        std::vector<unsigned char> objectMap = std::vector<unsigned char>(1024 * 1024);
        std::vector<unsigned short> bitflagMap = std::vector<unsigned short>(1024 * 1024);
        std::vector<std::vector<unsigned char>> lightingMap = {
            std::vector<unsigned char>(1024 * 1024),
            std::vector<unsigned char>(1024 * 1024),
            std::vector<unsigned char>(1024 * 1024)
        };
        std::vector<unsigned char> waterMap = std::vector<unsigned char>(1024 * 1024);
        std::vector<unsigned char> objectHeightMap = std::vector<unsigned char>(1024 * 1024);
        std::vector<unsigned char> fogMap = std::vector<unsigned char>(512 * 512);
        std::vector<unsigned char> ambientMap = std::vector<unsigned char>(512 * 512);
    };


    enum Bitflags {
        /**
         * These are for the car and 3df models
         */
        SF_DOUBLE_SIDE = 0x0001,
        SF_DARK_BACK = 0x0002,
        SF_OPACITY = 0x0004,
        SF_TRANSPARENT = 0x0008,
        SF_MORTAL = 0x0010,
        SF_PHONG = 0x0030,
        SF_ENV_MAP = 0x0050,
        SF_NEED_VC = 0x0080,
        SF_DARK = 0x8000,

        /**
         * These are used for the flagsMap in the Map data structure
         */
        BF_TEXTURE_DIRECTION = 0x0001,
        BF_B = 0x0002,
        BF_MODEL_DIRECTION = 0x0004,
        BF_D = 0x0008,
        BF_REVERSE = 0x0010,
        BF_IMPASSABLE = 0x0020,
        BF_G = 0x0040,
        BF_WATER = 0x0080,
        BF_TEXTURE2_DIRECTION = 0x0100,
        BF_J = 0x0200,
        BF_K = 0x0400,
        BF_L = 0x0800,
        BF_M = 0x1000,
        BF_N = 0x2000,
        BF_O = 0x4000,
        BF_WATER_UNUSED = 0x8000,
        BF_WATER_ALPHA = 0x8080,

        /**
         * objectmap flags for map models in the RSC file
         */
        OF_PLACE_WATER = 0x0001,
        OF_PLACE_GROUND = 0x0002,
        OF_PLACE_USER = 0x0004,
        OF_CIRCLE = 0x0008,
        OF_BOUND = 0x0016,
        OF_NO_BITMAP = 0x0032,
        OF_NO_LIGHT = 0x0064,
        OF_DEF_LIGHT = 0x0128,
        OF_GROUND_LIGHT = 0x0256,
        OF_NO_SOFT = 0x0512,
        OF_NO_SOFT2 = 0x1024,
        OF_ANIMATED = 0x80000000
    };


    struct Weapon {
        std::string name;
        std::string file;
        std::string pic;

        int power;
        float precision;
        float loudness;
        float rate;
        int shots;
    };

    struct Dino {
        std::string name;
        std::string file;

        int ai = -1;
        float mass = -1.0f;
        float length = -1.0f;
        int radius = -1;
        int health = -1;
        int baseScore = -1;

        float smell = -1.0f;
        float hearing = -1.0f;
        float sight = -1.0f;

        int scale0 = -1;
        int scaleA = -1;

        int shipDelta = -1;
        bool dangerous = false;
    };

    struct Prices {
        int baseCost = 100;

        std::vector<int> areas;
        std::vector<int> dinos;
        std::vector<int> weapons;
        std::vector<int> accessories;
    };

    struct ResTxt {
        std::vector<Weapon> weapons;
        std::vector<Dino> characters;

        Prices prices;
    };
}


#ifndef OCARN2_DEF
#ifdef OCARN2_STATIC
#define OCARN2_DEF static
#else
#define OCARN2_DEF extern
#endif
#endif


OCARN2_DEF OCARN2::Mesh load_3df_file(const char* filename);
OCARN2_DEF OCARN2::Mesh load_car_file(const char* filename);
OCARN2_DEF OCARN2::Rsc load_rsc_file(const char* filename);
OCARN2_DEF OCARN2::Map load_map_file(const char* filename);
OCARN2_DEF OCARN2::ResTxt load_res_txt_file(const char* filename);

OCARN2_DEF void free_mesh(OCARN2::Mesh& mesh);
OCARN2_DEF void free_rsc(OCARN2::Rsc& resources);


#ifdef OCARN2_IMPLEMENTATION



#include <fstream>
#include <functional>
#include <algorithm>
#include <cctype>
#include <locale>

/**
 * Internal function to open given filename, run callback, then close file
 * If file cannot be opened, error is printed to err stream
 *
 * @param filename
 * @param callback
 */
OCARN2_DEF void ocarn2__openFile(const char* filename, const std::function<void(std::fstream&)>& callback) {
    std::fstream file(filename, std::ios::in | std::ios::binary);
    if(file.is_open()) {

        callback(file);

        file.close();
    }
    else {
        std::cerr << "Unable to open " << filename << std::endl;
    }
}


/**
 * Internal function to load vertex from 3df file pointer
 *
 * @param in
 * @return
 */
OCARN2_DEF OCARN2::Vertex ocarn2__load_3df_vertex(std::fstream& in) {
    OCARN2::Vertex vertex {};

    in.read((char*) &vertex.x, 4);
    in.read((char*) &vertex.y, 4);
    in.read((char*) &vertex.z, 4);

    in.read((char*) &vertex.owner, 2);
    in.read((char*) &vertex.hidden, 2);

    return vertex;
}

/**
 * Internal function to load 3df node from file pointer
 *
 * @param in
 * @return
 */
OCARN2_DEF OCARN2::Node ocarn2__load_3df_node(std::fstream& in) {
    OCARN2::Node node {};

    in.read(node.name, 32);

    in.read((char*) &node.x, 4);
    in.read((char*) &node.y, 4);
    in.read((char*) &node.z, 4);

    in.read((char*) &node.owner, 2);
    in.read((char*) &node.hidden, 2);

    return node;
}

/**
 * Internal function to load face from 3df file pointer
 *
 * @param in
 * @return
 */
OCARN2_DEF OCARN2::Face ocarn2__load_3df_face(std::fstream& in) {
    OCARN2::Face face {};

    in.read((char*) &face, 64);

/*
    in.read((char*) &face.v1, 4);
    in.read((char*) &face.v2, 4);
    in.read((char*) &face.v3, 4);

    in.read((char*) &face.tax, 4);
    in.read((char*) &face.tbx, 4);
    in.read((char*) &face.tcx, 4);

    in.read((char*) &face.tay, 4);
    in.read((char*) &face.tby, 4);
    in.read((char*) &face.tcy, 4);

    in.read((char*) &face.flags, 2);
    in.read((char*) &face.DMask, 2);

    in.read((char*) &face.distant, 4);
    in.read((char*) &face.next, 4);
    in.read((char*) &face.group, 4);

    char buffer[255];
    in.read(buffer, 12);*/

    return face;
}

/**
 * Loads data from the given .3DF file
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::Mesh load_3df_file(const char* filename) {
    OCARN2::Mesh mesh {};

    /**
     * helper function do logging if filename is bad, and provide automatic closing of file when finished
     */
    ocarn2__openFile(filename, [&](std::fstream& file) {
        file.read( (char*) &mesh.numVertices, 4 );
        file.read( (char*) &mesh.numFaces,  4 );
        file.read( (char*) &mesh.numNodes, 4 );
        file.read( (char*) &mesh.textureSize, 4 );

        // load faces
        for(auto i=0; i < mesh.numFaces; i++) {
            mesh.faces.emplace_back(ocarn2__load_3df_face(file) );
        }

        // load vertices
        for(auto i=0; i < mesh.numVertices; i++) {
            mesh.vertices.emplace_back(ocarn2__load_3df_vertex(file) );
        }

        // load nodes
        for(auto i=0; i < mesh.numNodes; i++) {
            mesh.nodes.emplace_back(ocarn2__load_3df_node(file) );
        }

        // load texture
        mesh.textureData = new unsigned short[mesh.textureSize];
        file.read((char*) mesh.textureData, mesh.textureSize);
    });

    return mesh;
}

/**
 * Internal function to load animation from car file
 *
 * @param file
 * @param numVertices Number of Vertices in the Mesh. maybe pass mesh instead?
 * @return
 */
OCARN2_DEF OCARN2::Animation ocarn2__load_car_animation(std::fstream& file, unsigned const int numVertices) {
    OCARN2::Animation animation {};

    file.read(animation.name, 32);
    file.read((char*) &animation.kps, 4);
    file.read((char*) &animation.numFrames, 4);

    unsigned int size = numVertices * animation.numFrames * 6;
    animation.data = new short[size]; // (unsigned short*) malloc(size);
    file.read((char*) animation.data, size);

    return animation;
}

/**
 * Internal function to load sound effect associated with car model
 *
 * @param file
 * @return
 */
OCARN2_DEF OCARN2::SoundEffect ocarn2__load_car_sfx(std::fstream& file) {
    OCARN2::SoundEffect effect {};

    file.read(effect.name, 32);
    file.read((char*) &effect.length, 4);

    effect.data = new unsigned short[effect.length]; // (unsigned short*) malloc(effect.length);
    file.read((char*) effect.data, effect.length);

    return effect;
}


/**
 * Loads the Mesh found in the car file
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::Mesh load_car_file(const char* filename) {
    OCARN2::Mesh mesh;

    ocarn2__openFile(filename, [&](std::fstream& file) {
        file.read(mesh.name, 24);
        file.read(mesh.msc, 8);

        file.read((char*) &mesh.numAnimations, 4);
        file.read((char*) &mesh.numSoundEffects, 4);
        file.read((char*) &mesh.numVertices, 4);
        file.read((char*) &mesh.numFaces, 4);
        file.read((char*) &mesh.textureSize, 4);

        // load faces
        for(auto i=0; i < mesh.numFaces; i++) {
            mesh.faces.emplace_back(ocarn2__load_3df_face(file) );
        }

        // load vertices
        for(auto i=0; i < mesh.numVertices; i++) {
            mesh.vertices.emplace_back(ocarn2__load_3df_vertex(file) );
        }

        // load texture
        mesh.textureData = new unsigned short[mesh.textureSize]; // (unsigned short*) malloc(mesh.textureSize);
        file.read((char*) mesh.textureData, mesh.textureSize);

        // read animations
        for(auto i=0; i < mesh.numAnimations; i++) {
            mesh.animations.emplace_back(ocarn2__load_car_animation(file, mesh.numVertices) );
        }

        // read sounds
        for(auto i=0; i < mesh.numSoundEffects; i++) {
            mesh.soundEffects.emplace_back( ocarn2__load_car_sfx(file) );
        }

        // read sound map
        file.read((char*) &mesh.soundMap[0], 64 * 4);
    });

    return mesh;
}


// map and rsc definitions
/**
 * Internal function to load texture from rsc file's current position
 *
 * @param file
 * @return
 */
OCARN2_DEF OCARN2::Texture ocarn2__load_rsc_texture(std::fstream& file) {
    OCARN2::Texture texture {};

    texture.size = 128 * 128 * 2;
    texture.data = new unsigned short[texture.size]; // (unsigned short*) malloc(texture.size);
    file.read((char*) texture.data, texture.size);

    return texture;
}

/**
 *
 * @param file
 * @return
 */
OCARN2_DEF OCARN2::Mesh ocarn2__load_rsc_mesh(std::fstream& file) {
    OCARN2::Mesh mesh {};

    file.read((char*) &mesh.numVertices, 4);
    file.read((char*) &mesh.numFaces, 4);
    file.read((char*) &mesh.numNodes, 4);
    file.read((char*) &mesh.textureSize, 4);


    // load faces
    for(auto i=0; i < mesh.numFaces; i++) {
        mesh.faces.emplace_back(ocarn2__load_3df_face(file) );
    }

    // load vertices
    for(auto i=0; i < mesh.numVertices; i++) {
        mesh.vertices.emplace_back(ocarn2__load_3df_vertex(file) );
    }

    // load nodes
    for(auto i=0; i < mesh.numNodes; i++) {
        mesh.nodes.emplace_back(ocarn2__load_3df_node(file) );
    }

    // load texture
    mesh.textureData = new unsigned short[mesh.textureSize]; // (unsigned short*) malloc(mesh.textureSize);
    file.read((char*) mesh.textureData, mesh.textureSize);

    return mesh;
}

/**
 * Internal function to load an RSC formatted model from a filestream. Very similar to the 3df method
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::RscModel ocarn2__load_rsc_model(std::fstream &file) {
    OCARN2::RscModel model {};

    // read header
    file.read((char*) &model.radius, 4);
    file.read((char*) &model.yLo, 4);
    file.read((char*) &model.yHi, 4);
    file.read((char*) &model.lineLength, 4);
    file.read((char*) &model.lightIntensity, 4);
    file.read((char*) &model.circleRadius, 4);
    file.read((char*) &model.circleIntensity, 4);

    file.read((char*) &model.flags, 4);

    file.read((char*) &model.grRadius, 4);
    file.read((char*) &model.defLight, 4);
    file.read((char*) &model.lastAnimationTime, 4);
    file.read((char*) &model.boundingRadius, 4);

    file.read(model.reserved, 16);

    // done reading header
    // read mesh

    OCARN2::Mesh mesh {};
    file.read( (char*) &mesh.numVertices, 4 );
    file.read( (char*) &mesh.numFaces,  4 );
    file.read( (char*) &mesh.numNodes, 4 );
    file.read( (char*) &mesh.textureSize, 4 );

    // load faces
    for(auto i=0; i < mesh.numFaces; i++) {
        mesh.faces.emplace_back(ocarn2__load_3df_face(file) );
    }

    // load vertices
    for(auto i=0; i < mesh.numVertices; i++) {
        mesh.vertices.emplace_back(ocarn2__load_3df_vertex(file) );
    }

    // load nodes
    for(auto i=0; i < mesh.numNodes; i++) {
        mesh.nodes.emplace_back(ocarn2__load_3df_node(file) );
    }

    // load texture
    mesh.textureData = new unsigned short[mesh.textureSize];
    file.read((char*) mesh.textureData, mesh.textureSize);

    model.mesh = mesh;

    // done reading mesh
    // read texture?

    model.textureData = new unsigned short[model.textureSize];
    file.read((char*) model.textureData, model.textureSize);

    // end texture
    // read animations, if any
    if(model.flags & 0x80000000) {
        OCARN2::Animation animation {};
        int vc;

        file.read((char*) &vc, 4);
        file.read((char*) &vc, 4);
        file.read((char*) &animation.kps, 4);
        file.read((char*) &animation.numFrames, 4);

        int size = vc * animation.numFrames * 6;
        animation.data = new short[size]; // (unsigned short*) malloc( size );
        file.read((char*) animation.data, size);
    }
    return model;
}

/**
 * Loads all the data in an .rsc file into a self contained struct
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::Rsc load_rsc_file(const char* filename) {
    OCARN2::Rsc rsc;

    ocarn2__openFile(filename, [&](std::fstream& file) {
        file.read((char*) &rsc.numTextures, 4);
        file.read((char*) &rsc.numModels, 4);

        file.read((char*) rsc.fadeRgb, 4 * 3 * 3);
        file.read((char*) rsc.transRgb, 4 * 3 * 3);

        // load textures
        for(auto i=0; i < rsc.numTextures; i++) {
            rsc.textures.emplace_back( ocarn2__load_rsc_texture(file) );
        }

        // load models
        for(auto i=0; i < rsc.numModels; i++) {
            rsc.models.emplace_back( ocarn2__load_rsc_model(file) );
        }

        // load sky textures
        file.read((char*) rsc.sky[0], 256 * 256 * 2);
        file.read((char*) rsc.sky[1], 256 * 256 * 2);
        file.read((char*) rsc.sky[2], 256 * 256 * 2);

        // load cloudsMap
        file.read((char*) rsc.skyMap, 128 * 128);

        // load fogs map
        file.read((char*) &rsc.numFogs, 4);
        for(auto i=0; i < rsc.numFogs; i++) {
            OCARN2::Fog fog {};

            file.read((char*) &fog.rgb, 4);

            fog.color.a = 127; // fog will always be solid
            fog.color.r = (fog.rgb >> 16) & 255;
            fog.color.g = (fog.rgb >> 8) & 255;
            fog.color.b = (fog.rgb) & 255;


            file.read((char*) &fog.y, 4);
            file.read((char*) &fog.isMortal, 4);
            file.read((char*) &fog.transparent, 4);
            file.read((char*) &fog.fLimit, 4);

            rsc.fogs.emplace_back(fog);
        }

        // load random sounds
        file.read((char*) &rsc.numSoundEffects, 4);
        for(auto i=0; i < rsc.numSoundEffects; i++) {
            OCARN2::SoundEffect sound {};

            file.read((char*) &sound.length, 4);
            sound.data = new unsigned short[sound.length];
            file.read((char*) sound.data, sound.length);

            rsc.soundEffects.emplace_back(sound);
        }

        // load ambient sounds
        file.read((char*) &rsc.numAmbientSounds, 4);
        for(auto i=0; i < rsc.numAmbientSounds; i++) {
            OCARN2::AmbientSound ambient {};

            file.read((char*) &ambient.sound.length, 4);

            ambient.sound.data = new unsigned short[ambient.sound.length];
            file.read((char*) ambient.sound.data, ambient.sound.length);

            file.read((char*) ambient.randomSounds, sizeof(ambient.randomSounds));
            file.read((char*) &ambient.numSoundEffects, 4);
            file.read((char*) &ambient.volume, 4);

            rsc.ambientSounds.emplace_back(ambient);
        }

        // load water table
        file.read((char*) &rsc.numWaters, 4);
        for(auto i=0; i < rsc.numWaters; i++) {
            OCARN2::Water water {};

            file.read((char*) &water.tIndex, 4);
            file.read((char*) &water.wLevel, 4);
            file.read((char*) &water.transparency, 4);
            file.read((char*) &water.rgb, 4);

            water.color.a = 0;
            water.color.r = water.rgb >> 16;
            water.color.g = water.rgb >> 8;
            water.color.b = water.rgb >> 0;

            rsc.waters.emplace_back(water);
        }
    });

    return rsc;
}


/**
 * Reads a map file into a self contained struct
 *
 * All fields, except fog and ambient maps, are 1024 * 1024, single dimensional vectors
 * fog and ambient maps are 512 * 512
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::Map load_map_file(const char* filename) {
    OCARN2::Map map {};

    ocarn2__openFile(filename, [&](std::fstream& file) {
        file.read((char*) &map.heightMap[0],       1024 * 1024);
        file.read((char*) &map.textureMap[0],      1024 * 1024 * 2);
        file.read((char*) &map.textureMapFar[0],   1024 * 1024 * 2);
        file.read((char*) &map.objectMap[0],       1024 * 1024);
        file.read((char*) &map.bitflagMap[0],      1024 * 1024 * 2);
        file.read((char*) &map.lightingMap[0][0],  1024 * 1024); // dawn
        file.read((char*) &map.lightingMap[1][0],  1024 * 1024); // noon
        file.read((char*) &map.lightingMap[2][0],  1024 * 1024); // night
        file.read((char*) &map.waterMap[0],        1024 * 1024);
        file.read((char*) &map.objectHeightMap[0], 1024 * 1024);
        file.read((char*) &map.fogMap[0],          512 * 512);
        file.read((char*) &map.ambientMap[0],      512 * 512);
    });

    return map;
}



// trim from start
static inline std::string &ocarn2__ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &ocarn2__rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &ocarn2__trim(std::string &s) {
    return ocarn2__ltrim(ocarn2__rtrim(s));
}



OCARN2_DEF OCARN2::Weapon ocarn2__load_weapon(std::fstream& file, std::string& line) {
    OCARN2::Weapon weapon;

    std::string key, value;

    // load individual weapon block
    while(std::getline(file, line) && !line.empty()) {
        if( line.find('}') != std::string::npos ) { break; } // end subloop if we encounter a close brace

        key = line.substr(0, line.find('='));
        value = line.substr( line.find('=') + 1 );

        ocarn2__trim(key);
        ocarn2__trim(value);

        // set properties
        if(key == "name") weapon.name = value;
        if(key == "file") weapon.file = value;
        if(key == "pic") weapon.pic = value;
        if(key == "power") weapon.power = std::stoi(value);
        if(key == "prec") weapon.precision = std::stod(value);
        if(key == "loud") weapon.loudness = std::stod(value);
        if(key == "rate") weapon.rate = std::stod(value);
        if(key == "shots") weapon.shots = std::stoi(value);
    }

    return weapon;
}


OCARN2_DEF OCARN2::Dino ocarn2__load_character(std::fstream& file, std::string& line) {
    OCARN2::Dino character;

    std::string key, value;

    // load individual character block
    while(std::getline(file, line) && !line.empty()) {
        if( line.find('}') != std::string::npos ) { break; } // end subloop if we encounter a close brace

        key = line.substr(0, line.find('='));
        value = line.substr( line.find('=') + 1 );

        ocarn2__trim(key);
        ocarn2__trim(value);


        // set properties
        if(key == "name") character.name = value;
        if(key == "file") character.file = value;
        if(key == "ai") character.ai = std::stoi(value);
        if(key == "mass") character.mass = std::stod(value);
        if(key == "length") character.length = std::stod(value);
        if(key == "radius") character.radius = std::stoi(value);
        if(key == "health") character.health = std::stoi(value);
        if(key == "basescore") character.baseScore = std::stoi(value);
        if(key == "smell") character.smell = std::stod(value);
        if(key == "hear") character.hearing = std::stod(value);
        if(key == "look") character.sight = std::stod(value);
        if(key == "shipdelta") character.shipDelta = std::stoi(value);
        if(key == "scale0") character.scale0 = std::stoi(value);
        if(key == "scaleA") character.scaleA = std::stoi(value);


        // convert TRUE -> true for comparison
        if(key == "danger") {
            std::transform(value.begin(), value.end(), value.begin(), std::ptr_fun<int,int>(std::tolower));

            character.dangerous = (value == "true");
        };


        // these are typos left over in the resources
        // should be "hear" and "look", so if the default value is still set, allow the typo to set the value
        if(key == "hearK" && character.hearing == -1.0f)
            character.hearing = std::stod(value);

        if(key == "lookK" && character.sight == -1.0f)
            character.sight = std::stod(value);
    }

    return character;
}


/**
 * Loads the _RES.TXT file in HUNTDAT for custom configuration
 *
 * @param filename
 * @return
 */
OCARN2_DEF OCARN2::ResTxt load_res_txt_file(const char* filename) {
    OCARN2::ResTxt resources;

    ocarn2__openFile(filename, [&](std::fstream& file) {

        while(!file.eof()) {
            std::string line;

            std::getline(file, line, '\n');
            // process lines


            if( line.find("weapons") != std::string::npos ) {

                // loop through weapons {}
                while( std::getline(file, line) && !line.empty() ) {
                    if( line.find('}') != std::string::npos ) break; // end subloop if we encounter a close brace

                    // if we find an opening brace,
                    if(line.find('{') != std::string::npos) {

                        // load weapon into struct
                        resources.weapons.emplace_back( ocarn2__load_weapon(file, line) );
                    }
                }
            }

            // loop through dinos
            if(line.find("characters") != std::string::npos ) {

                // loop through characters {}
                while( std::getline(file, line) && !line.empty() ) {
                    if( line.find('}') != std::string::npos ) break; // end subloop if we encounter a close brace

                    // if we find an opening brace,
                    if(line.find('{') != std::string::npos) {

                        // load character into struct
                        resources.characters.emplace_back( ocarn2__load_character(file, line) );
                    }
                }
            }


            // loop through prices
            if( line.find("prices") != std::string::npos ) {

                while( std::getline(file, line) && !line.empty() ) {
                    if( line.find('}') != std::string::npos ) break;

                    std::string key, value;

                    key = line.substr(0, line.find('='));
                    value = line.substr( line.find('=') + 1 );

                    ocarn2__trim(key);
                    ocarn2__trim(value);

                    if(key == "start") resources.prices.baseCost = std::stoi(value);

                    if(key == "area") resources.prices.areas.push_back( std::stoi(value) );
                    if(key == "dino") resources.prices.dinos.push_back( std::stoi(value) );
                    if(key == "weapon") resources.prices.weapons.push_back( std::stoi(value) );
                    if(key == "acces") resources.prices.accessories.push_back( std::stoi(value) );
                }

            }
        }

    });

    return resources;
}


// functions to clean up memory at the end of program

/**
 * deletes all the pointers that were allocated in creating the mesh
 *
 * @param mesh
 */
OCARN2_DEF void free_mesh(OCARN2::Mesh& mesh) {
    delete mesh.textureData;

    for(auto& a: mesh.animations) {
        delete a.data;
    }

    for(auto& s: mesh.soundEffects) {
        delete s.data;
    }
}

/**
 * frees all the pointers that were created when rsc file was loaded
 *
 * @param resources
 */
OCARN2_DEF void free_rsc(OCARN2::Rsc& resources) {

    for(auto& s: resources.soundEffects)
        delete s.data;

    for(auto& t: resources.textures)
        delete t.data;

    for(auto& a: resources.ambientSounds)
        delete a.sound.data;

    for(auto& m: resources.models) {
        delete m.textureData;

        free_mesh(m.mesh);
    }
}

#endif