#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "helper_functions.h"

using std::string;
using std::vector;
using std::normal_distribution;

//Define static generator to use across multiple functions
static std::default_random_engine gen;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * Sets the number of particles. Initialize all particles to
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1.
   * Adds random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method
   *   (and others in this file).
   */
   num_particles = 100;

   //Initialize all weights to 1
   weights = std::vector<double>(num_particles, 1.0);

   //Creates a Gaussian Distribution for x, y, and theta
   normal_distribution<double> dist_x(x, std[0]);
   normal_distribution<double> dist_y(y, std[1]);
   normal_distribution<double> dist_theta(theta, std[2]);

   //Initializes each particle
   for(int i=0; i<num_particles; i++) {
     Particle particle;
     particle.x = dist_x(gen);
     particle.y = dist_y(gen);
     particle.theta = dist_theta(gen);
     particle.weight = weights[i];
     particle.id = i;

     particles.push_back(particle);
   }

   //Set initialization flag
   is_initialized = true;

}

void ParticleFilter::prediction(double delta_t, double std_pos[],
                                double velocity, double yaw_rate) {
  /**
   * Adds measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */

   //Create Guassian Distrubtion for x, y, and theta
   normal_distribution<double> dist_x(0.0, std_pos[0]);
   normal_distribution<double> dist_y(0.0, std_pos[1]);
   normal_distribution<double> dist_theta(0.0, std_pos[2]);

   for(int i=0; i<num_particles; i++) {
     //If the vehicle is not changing direction, use these equations to calculate x and y
     if(fabs(yaw_rate) < 0.00001) {
       particles[i].x += velocity * delta_t * cos(particles[i].theta);
       particles[i].y += velocity * delta_t * sin(particles[i].theta);
     }
     else { //If the vehicle's yaw rate is not zero, use these equations to find the new x, y, and theta values
       particles[i].x += velocity / yaw_rate * (sin(particles[i].theta + yaw_rate*delta_t) - sin(particles[i].theta));
       particles[i].y += velocity / yaw_rate * (cos(particles[i].theta) - cos(particles[i].theta + yaw_rate*delta_t));
       particles[i].theta += yaw_rate * delta_t;
     }

     //Add noise to the newly predicted particles
     particles[i].x += dist_x(gen);
     particles[i].y += dist_y(gen);
     particles[i].theta += dist_theta(gen);
   }

}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted,
                                     vector<LandmarkObs>& observations) {
  /**
   * Finds the predicted measurement that is closest to each
   *   observed measurement and assign the observed measurement to this
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will
   *   probably find it useful to implement this method and use it as a helper
   *   during the updateWeights phase.
   */

   for(int i=0; i<observations.size(); i++) {

     LandmarkObs obs = observations[i];

     //Set min_dist to a large number to begin with
     double min_dist = std::numeric_limits<double>::max();

     //Holds the ID of the particle closest to the landmark
     int map_id = -1;

     for(int j=0; j<predicted.size(); j++) {

       LandmarkObs pred = predicted[j];

       //Find the distance between the two points
       double currDist = dist(obs.x, obs.y, pred.x, pred.y);

       //If the current predicted is less than min_dist, update the min_dist
       if (currDist < min_dist) {
         min_dist = currDist;
         map_id = pred.id;
       }
     }
     //Save the ID of the nearest particle to the landmark
     observations[i].id = map_id;
   }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[],
                                   const vector<LandmarkObs> &observations,
                                   const Map &map_landmarks) {
  /**
   * Updates the weights of each particle using a mult-variate Gaussian
   *   distribution. You can read more about this distribution here:
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system.
   *   Your particles are located according to the MAP'S coordinate system.
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */

   for(int i=0; i<num_particles; i++) {

     double p_x = particles[i].x;
     double p_y = particles[i].y;
     double p_theta = particles[i].theta;

     //Create Vector to add predictions
     vector<LandmarkObs> predictions;

     for(int j=0; j<map_landmarks.landmark_list.size(); j++) {

       float lm_x = map_landmarks.landmark_list[j].x_f;
       float lm_y = map_landmarks.landmark_list[j].y_f;
       int lm_id = map_landmarks.landmark_list[j].id_i;

       //Only consider landmarks within sensor range of the particle
       if(fabs(p_x - lm_x) <= sensor_range && fabs(p_y - lm_y) <= sensor_range) {
         //Add landmark to predictions
         predictions.push_back(LandmarkObs{lm_id, lm_x, lm_y});
       }

     }

     //Transform observations from vehicle to map coordinates
     vector<LandmarkObs> map_observations;

     for (int j=0; j<observations.size(); j++) {

       //Transfrom vehicle x & y to map x & y
       double x_map = p_x + (cos(p_theta) * observations[j].x) - (sin(p_theta) * observations[j].y);
       double y_map = p_y + (sin(p_theta) * observations[j].x) + (cos(p_theta) * observations[j].y);

       //Append transformed landmark to new observation vector
       map_observations.push_back(LandmarkObs{observations[j].id, x_map, y_map});
     }

     //Perfrom Data Associations on current particle (See function above)
     dataAssociation(predictions, map_observations);

     //Reinitialize weight
     particles[i].weight = 1.0;

     for(int j=0; j<map_observations.size(); j++) {

       double obs_x, obs_y, pred_x, pred_y;

       obs_x = map_observations[j].x;
       obs_y = map_observations[j].y;

       //Get ID of the landmark's nearest particle found in dataAssociation
       int prediction_id = map_observations[j].id;

       for(int k=0; i<predictions.size(); k++) {
         //If ID matches, save the x & Y values of the particle(prediction)
         if(predictions[k].id == prediction_id) {
           pred_x = predictions[k].x;
           pred_y = predictions[k].y;

           //No need to continue looping if already found
           break;
         }
       }

       //Get sigma x & y from function parameters
       double sig_x = std_landmark[0];
       double sig_y = std_landmark[1];

       //Calculate Multivariate Guassian Distribution to determine weights
       //Calculate normalization term
       double gauss_norm = 1 / (2 * M_PI * sig_x * sig_y);

       //Calculate exponent
       double exponent = (pow(pred_x - obs_x, 2) / (2 * pow(sig_x, 2))) + (pow(pred_y - obs_y, 2) / (2 * pow(sig_y, 2)));

       //Calculate weight using normalization terms and exponent
       double weight = gauss_norm * exp(-exponent);

       //Product of this observation's weight and the total observation weight
       particles[i].weight *= weight;
     }
   }
}

void ParticleFilter::resample() {
  /**
   * Resamples particles with replacement with probability proportional
   *   to their weight.
   * NOTE: You may find std::discrete_distribution helpful here.
   *   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
   */

   vector<Particle> resampled_particles;

   vector<double> weights;

   //Add all particle weights to a vector
   for(int i=0; i<num_particles; i++) {
     weights.push_back(particles[i].weight);
   }

   //Use a generator to get a random starting index for the resampling wheel
   std::uniform_int_distribution<int> uniintdist(0, num_particles-1);
   int index = uniintdist(gen);

   //Get max weight
   double max_weight = *std::max_element(weights.begin(), weights.end());

   //Create a unifrom real Distribution
   std::uniform_real_distribution<double> unirealdist(0, max_weight);

   double beta = 0.0;

   for(int i=0; i<num_particles; i++) {

     beta += unirealdist(gen) * 2.0;

     while(beta > weights[index]) {
       beta -= weights[index];
       index = (index + 1) % num_particles;
     }

     resampled_particles.push_back(particles[index]);
   }

   //Replace the particles vector with the resampled particles
   particles = resampled_particles;
}

void ParticleFilter::SetAssociations(Particle& particle,
                                     const vector<int>& associations,
                                     const vector<double>& sense_x,
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association,
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}