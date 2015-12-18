#include <LinearMath/btVector3.h>//
// Created by Abigail Coleman 10/28/15
//

#include <iostream>
#include <math.h>

#include <opencv2/opencv.hpp>

#include "volumepkg.h"
#include "vc_defines.h"
#include "vc_datatypes.h"
#include "io/ply2itk.h"
#include "io/objWriter.h"
#include "compositeTexture.h"

// bullet converter
#include "itk2bullet.h"

struct NodeTarget {
    btVector3 t_pos;
    btScalar  t_stepsize;
};

btVector3 middle(0,0,0);

btVector3 btAverageNormal( btSoftBody* body );
double btSurfaceArea( btSoftBody* body );
static void planarizeCornersPreTickCallback(btDynamicsWorld *world, btScalar timeStep);
static void emptyPreTickCallback(btDynamicsWorld *world, btScalar timeStep);
void expandCorners(float magnitude);
std::vector<btSoftBody::Node*> pinnedPoints;
std::vector<NodeTarget> targetPoints;

int main(int argc, char* argv[]) {
    if ( argc < 5 ) {
        std::cout << "Usage: vc_texture2 volpkg seg-id iterations" << std::endl;
    }

    VolumePkg vpkg = VolumePkg( argv[ 1 ] );
    std::string segID = argv[ 2 ];
    if (segID == "") {
    std::cerr << "ERROR: Incorrect/missing segmentation ID!" << std::endl;
    exit(EXIT_FAILURE);
    }
    if ( vpkg.getVersion() < 2.0) {
    std::cerr << "ERROR: Volume package is version " << vpkg.getVersion() << " but this program requires a version >= 2.0."  << std::endl;
    exit(EXIT_FAILURE);
    }
    vpkg.setActiveSegmentation(segID);
    std::string meshName = vpkg.getMeshPath();

    int64_t NUM_OF_ITERATIONS = atoi( argv[ 3 ] );

    // declare pointer to new Mesh object
    VC_MeshType::Pointer  mesh = VC_MeshType::New();

    int meshWidth = -1;
    int meshHeight = -1;

    // try to convert the ply to an ITK mesh
    if (!volcart::io::ply2itkmesh(meshName, mesh, meshWidth, meshHeight)) {
        exit( -1 );
    };

    //volcart::Texture newTexture;
    //newTexture = volcart::texturing::compositeTexture(mesh, vpkg, meshWidth, meshHeight, 7, VC_Composite_Option::Maximum, VC_Direction_Option::Bidirectional);

    // Create Dynamic world for bullet cloth simulation
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();

    btDefaultCollisionConfiguration* collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

    btSoftBodySolver* softBodySolver = new btDefaultSoftBodySolver();

    btSoftRigidDynamicsWorld* dynamicsWorld = new btSoftRigidDynamicsWorld( dispatcher,
                                                                          broadphase,
                                                                          solver,
                                                                          collisionConfiguration,
                                                                          softBodySolver);

    dynamicsWorld->setGravity(btVector3(0, 0, 0));

    // convert itk mesh to bullet mesh (vertices and triangle arrays)
    std::cerr << "volcart::cloth::message: Converting mesh to softBody" << std::endl;
    btSoftBody* psb;
    volcart::meshing::itk2bullet::itk2bullet(mesh, dynamicsWorld->getWorldInfo(), &psb);

    psb->getWorldInfo()->m_gravity = dynamicsWorld->getGravity(); // Have to explicitly make softbody gravity match world gravity
    dynamicsWorld->addSoftBody(psb);

    // Constraints for the mesh as a soft body
    // These needed to be tested to find optimal values.
    // Sets the mass of the whole soft body, true considers the faces along with the vertices
    // Note: Mass is in kilograms. If mass isn't high enough, nothing changes.
    std::cerr << "volcart::cloth::message: Setting mass" << std::endl;
    psb->setTotalMass( (int)(psb->m_nodes.size() * 0.001), true );

    psb->m_cfg.kDP = 0.1; // Damping coefficient of the soft body [0,1]
    psb->m_materials[0]->m_kLST = 1.0; // Linear stiffness coefficient [0,1]
    psb->m_materials[0]->m_kAST = 1.0; // Area/Angular stiffness coefficient [0,1]
    psb->m_materials[0]->m_kVST = 1.0; // Volume stiffness coefficient [0,1]

    // Find the position of the four corner nodes
    // Currently assumes that the first point has the same z-value as the rest of the starting chain
    int min_z = (int) std::floor(mesh->GetPoint(0)[2]);
    int chain_size = 1;
    double chain_length = 0;
    // Calculate chain size and chain length
    for(int i = 1; i < psb->m_nodes.size(); ++i) {
        if( (int)psb->m_nodes[i].m_x.z() <= min_z ) {
            chain_length += psb->m_nodes[i].m_x.distance(psb->m_nodes[i-1].m_x);
            ++chain_size;
        }
        else
            break;
    }

    // For debug. These values should match.
    if ( chain_size != meshWidth ) return EXIT_FAILURE;

    // Append rigid bodies to respective nodes of mesh
    // Assumes the chain length is constant throughout the mesh
    btSoftBody::Node* top_left     = &psb->m_nodes[0];
    btSoftBody::Node* top_right    = &psb->m_nodes[chain_size - 1];
    btSoftBody::Node* bottom_left  = &psb->m_nodes[psb->m_nodes.size() - chain_size];
    btSoftBody::Node* bottom_right = &psb->m_nodes[psb->m_nodes.size() - 1];

    pinnedPoints.push_back(top_left);
    pinnedPoints.push_back(top_right);
    pinnedPoints.push_back(bottom_left);
    pinnedPoints.push_back(bottom_right);

    double pinMass = 10;
    psb->setMass(0, pinMass);
    psb->setMass(chain_size - 1, pinMass);
    psb->setMass(psb->m_nodes.size() - chain_size, pinMass);
    psb->setMass(psb->m_nodes.size() - 1, pinMass);

    // Calculate the surface area of the mesh
    double surface_area = btSurfaceArea(psb);
    int dir = ( top_left->m_x.getX() < top_right->m_x.getX() ) ? 1 : -1;
    double width = chain_length * dir;
    double height = surface_area / chain_length;
    int required_iterations = NUM_OF_ITERATIONS; // Minimum iterations to reach target
    std::cout << "Chain size: " << chain_size << " | Plane Dimensions: " << width << "x" << height << " | Surface area: " << surface_area << std::endl;

    // Create target positions with step size for our four corners
    // NOTE: Must be created in the same order that the rigid bodies were put into pinnedPoints
    NodeTarget n_target;
    btScalar t_x, t_y, t_z;
    btSoftBody::Node* node_ptr;

    // top left corner
    node_ptr = &psb->m_nodes[0];
    n_target.t_pos = psb->m_nodes[0].m_x;
    n_target.t_stepsize = (node_ptr)->m_x.distance(n_target.t_pos) / required_iterations;
    targetPoints.push_back(n_target);

    // top right corner
    node_ptr = &psb->m_nodes[chain_size - 1];
    t_x = psb->m_nodes[0].m_x.x() + width;
    t_y = psb->m_nodes[0].m_x.y();
    t_z = psb->m_nodes[0].m_x.z();
    n_target.t_pos = btVector3(t_x, t_y, t_z);
    n_target.t_stepsize = (node_ptr)->m_x.distance(n_target.t_pos) / required_iterations;
    targetPoints.push_back(n_target);

    // bottom left corner
    node_ptr = &psb->m_nodes[psb->m_nodes.size() - chain_size];
    t_x = psb->m_nodes[0].m_x.x();
    t_y = psb->m_nodes[0].m_x.y();
    t_z = psb->m_nodes[0].m_x.z() + height;
    n_target.t_pos = btVector3(t_x, t_y, t_z);
    n_target.t_stepsize = (node_ptr)->m_x.distance(n_target.t_pos) / required_iterations;
    targetPoints.push_back(n_target);

    // bottom right corner
    node_ptr = &psb->m_nodes[psb->m_nodes.size() - 1];
    t_x = psb->m_nodes[0].m_x.x() + width;
    t_y = psb->m_nodes[0].m_x.y();
    t_z = psb->m_nodes[0].m_x.z() + height;
    n_target.t_pos = btVector3(t_x, t_y, t_z);
    n_target.t_stepsize = (node_ptr)->m_x.distance(n_target.t_pos) / required_iterations;
    targetPoints.push_back(n_target);

    // Middle of target rectangle
    t_x = psb->m_nodes[0].m_x.x() + (width / 2);
    t_y = psb->m_nodes[0].m_x.y();
    t_z = psb->m_nodes[0].m_x.z() + (height / 2);
    middle = btVector3(t_x, t_y, t_z);

    // Planarize the corners
    std::cerr << "volcart::cloth::message: Planarizing corners" << std::endl;
    dynamicsWorld->setInternalTickCallback(planarizeCornersPreTickCallback, dynamicsWorld, true);
    for (int i = 0; i < required_iterations; ++i) {
        std::cerr << "volcart::cloth::message: Step " << i + 1 << "/" << required_iterations << "\r" << std::flush;
        dynamicsWorld->stepSimulation(1/60.f);
        psb->solveConstraints();
    }
    std::cerr << std::endl;

    // Expand the corners
    std::cerr << "volcart::cloth::message: Expanding corners" << std::endl;
    int i = 0;
    while ( btAverageNormal(psb).absolute().getY() < 0.9 ) {
        std::cerr << "volcart::cloth::message: Step " << i + 1 << "\r" << std::flush;
        if ( i % 2000 == 0 ) expandCorners( 10 + (i / 2000) );
        dynamicsWorld->stepSimulation(1/60.f);
        psb->solveConstraints();
        ++i;
    }

    std::cerr << std::endl;

    // Add a collision plane to push the mesh onto
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* plane = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(plane);

	// Set the gravity so the mesh will be pushed onto the plane
	dynamicsWorld->setGravity(btVector3(0, -25, 0));
	psb->getWorldInfo()->m_gravity = dynamicsWorld->getGravity(); // Have to explicitly make softbody gravity match world gravity

	// set the friction of the plane and the mesh s.t. the mesh can easily flatten upon collision
	plane->setFriction(0.1); // (0-1] Default: 0.5
	psb->m_cfg.kDF = 0.1; // Dynamic friction coefficient (0-1] Default: 0.2

    // Let it settle
    std::cerr << "volcart::cloth::message: Relaxing corners" << std::endl;
    dynamicsWorld->setInternalTickCallback(emptyPreTickCallback, dynamicsWorld, true);
    required_iterations = 20000;
    for (int j = 0; j < required_iterations; ++j) {
        std::cerr << "volcart::cloth::message: Step " << j + 1 << "/" << required_iterations << "\r" << std::flush;
        dynamicsWorld->stepSimulation(1/60.f);
        psb->solveConstraints();
    }
    std::cerr << std::endl;

    // UV map setup
    double u, v;
    int min_x = psb->m_nodes[0].m_x.x();
    min_z = psb->m_nodes[0].m_x.z();
    int max_x = psb->m_nodes[chain_size - 1].m_x.x();
    int max_z = psb->m_nodes[psb->m_nodes.size() - chain_size].m_x.z();
    width = max_x - min_x;
    height = max_z - min_z;
    double aspect = width / height;
    volcart::UVMap uvMap;

    // Calculate uv coordinates
    for ( size_t p_id = 0; p_id < psb->m_faces.size(); ++p_id ) {

        for( int j = 0; j < 3; ++j ) {

            u = ( psb->m_faces[p_id].m_n[j]->m_x.x() - min_x ) / ( max_x - min_x );
            v = ( psb->m_faces[p_id].m_n[j]->m_x.z() - min_z ) / ( max_z - min_z );

            cv::Vec2d uv( u, v );

            // Add the uv coordinates into our map at the point index specified
            uvMap.set( p_id, uv );

        }
    }

    // Convert soft body to itk mesh
    std::cerr << "volcart::cloth::message: Updating mesh" << std::endl;
    volcart::meshing::bullet2itk::bullet2itk(mesh, psb);

    //volcart::io::objWriter objwriter("cloth.obj", mesh, newTexture.uvMap(), newTexture.getImage(0));
    volcart::io::objWriter objwriter("cloth.obj", mesh);
    objwriter.write();

    // bullet clean up
    dynamicsWorld->removeRigidBody(plane);
	delete plane->getMotionState();
	delete plane;
	delete groundShape;
    dynamicsWorld->removeSoftBody(psb);
    delete psb;
    delete dynamicsWorld;
    delete softBodySolver;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;

    return 0;
} // end main

btVector3 btAverageNormal(btSoftBody* body) {
    btVector3  avg_normal(0,0,0);
    for ( size_t n_id = 0; n_id < body->m_faces.size(); ++n_id ) {
        avg_normal += body->m_faces[n_id].m_normal;
    }
    avg_normal /= body->m_faces.size();
    bool result = ( avg_normal.absolute().getZ() > 0.9 );
    return avg_normal;
};

// Calculate the surface area of the mesh using Heron's formula
// Let a,b,c be the lengths of the sides of a triangle and p the semiperimeter
// p = (a +  b + c) / 2
// area of triangle = sqrt( p * (p - a) * (p - b) * (p - c) )
double btSurfaceArea( btSoftBody* body ) {
    double surface_area = 0;
    for(int i = 0; i < body->m_faces.size(); ++i) {
        double a = 0, b = 0, c = 0, p = 0;
        a = body->m_faces[i].m_n[0]->m_x.distance(body->m_faces[i].m_n[1]->m_x);
        b = body->m_faces[i].m_n[0]->m_x.distance(body->m_faces[i].m_n[2]->m_x);
        c = body->m_faces[i].m_n[1]->m_x.distance(body->m_faces[i].m_n[2]->m_x);

        p = (a + b + c) / 2;

        surface_area += sqrt( p * (p - a) * (p - b) * (p - c) );
    }

    return surface_area;
}

void planarizeCornersPreTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    // Iterate over rigid bodies and move them towards their targets
    for( size_t p_id = 0; p_id < pinnedPoints.size(); ++p_id ) {
        if ( pinnedPoints[p_id]->m_x == targetPoints[p_id].t_pos ) continue;
        btVector3 delta = (targetPoints[p_id].t_pos - pinnedPoints[p_id]->m_x).normalized() * targetPoints[p_id].t_stepsize;
        pinnedPoints[p_id]->m_v += delta/timeStep;
    }
};

void emptyPreTickCallback(btDynamicsWorld *world, btScalar timeStep) {
	// This call back is used to disable other callbacks
	// Particularly used for relaxing the four corners
};

void expandCorners(float magnitude) {
    btScalar stepSize = 1;
    btScalar _magnitude = magnitude;
    for( size_t p_id = 0; p_id < pinnedPoints.size(); ++p_id ) {
        targetPoints[p_id].t_pos += (targetPoints[p_id].t_pos - middle).normalized() * _magnitude;
        targetPoints[p_id].t_stepsize = stepSize;
    }
}
