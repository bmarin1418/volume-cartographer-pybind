#pragma once

#define DEFAULT_OFFSET (-1)

#include <list>
#include <vector>

#include <opencv2/core.hpp>

#include "core/types/VolumePkg.hpp"
#include "segmentation/stps/Particle.hpp"
/**
 * @brief A chain of particles and their propagation histories
 *
 * Handles the basic chain propagation task for StructureTensorParticleSim().
 * Particles are propagated forward through the volume using a locally computed
 * structure tensor (generated by gravity()). Particles are kept together using
 * a simple Hooke's spring force equation.
 *
 * @ingroup stps
 */
class Chain
{
public:
    /**
     * @brief Construct using a chain of starting points
     * @param segPath Input seed points
     * @param volpkg The volume within which the chain will propagate
     * @param gravityScale Structure tensor scale factor
     * @param threshold Distance each particle must travel before new position
     * is sampled
     * @param endOffset Final offset from starting z-index. Default propagates
     * to last possible z-index.
     * @param springConstantK Spring constant between particles
     */
    Chain(
        std::vector<cv::Vec3d> segPath,
        const VolumePkg& volpkg,
        double gravityScale,
        int threshold,
        int endOffset,
        double springConstantK = -0.5);

    /** @brief Propagate particle chain forward one step */
    void step();

    /** @brief Return Moving status of each particle */
    bool isMoving();

    /**
     * @brief Return the corrective vector required to maintain the distance
     * between a particle and its neighbors
     *
     * This vector is generated from the equation of Hooke's law \f$F = -kx\f$
     * where k is the spring constant (stiffness) and x is the displacement
     * from resting position. Resting position is here defined to be the
     * initial distance between particles in the chain.
     *
     * @param index Index of particle within chain
     */
    cv::Vec3d springForce(size_t index);

    /**
     * @brief Return the propagation vector used to move a particle forward
     * through the volume
     *
     * This function first computes the local structure tensor using the volume
     * information adjacent to the particle using a neighborhood radius of 3.
     * The structure tensor is assumed to describe a planar region, and
     * therefore the first eigen vector is assumed to be normal to this plane.
     * The resulting vector is the vector rejection of a z-axis aligned unit
     * vector and this normal vector, scaled by Chain::gravityScale_.
     * \f{eqnarray*}{
        \vec{g} &=& <0,0,1> \\
        \vec{N} &=& \mbox{Normal computed from structure tensor} \\
        S &=& \mbox{Gravity scale factor} \\
        \vec{G} &=& (\vec{g} - \frac{\vec{g} \cdot \vec{N}}{\vec{N} \cdot
     N}\vec{N})S
     * \f}
     */
    cv::Vec3d gravity(size_t index);

    /** @brief Return the chain's propagation history as an OrderedPointSet */
    volcart::OrderedPointSet<cv::Vec3d> orderedPCD();

private:
    /** VolumePkg from which to pull intensity information */
    const VolumePkg& volpkg_;

    /** History of the chain at each iteration */
    std::list<std::vector<Particle>> history_;

    /** Spring force constant (stiffness) */
    double springConstantK_;

    /** Resting distance between particles */
    double springRestingX_;

    /** "Gravity" scale factor */
    double gravityScale_;

    /** @name Chain metadata */
    //@{
    /** Number of particles in the chain */
    size_t chainLength_;
    /** Number of sampled iterations */
    size_t realIterations_;
    /** Starting slice index */
    size_t startIndex_;
    /** Target slice index */
    size_t targetIndex_;
    /** Distance threshold at which to sample the chain (from previous sampled
     * position) */
    int threshold_;  // To-Do: What is this for now? We may not need this.
    //@}
};
