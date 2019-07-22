#pragma once
// PIDController
// by Joss Moo-Young

// This class implements a PID Controller

// PID stands for Proportional Integral Derivative. It takes feedback and an internal state and responds with outputs to achieve a goal state.

// This templated PID Controller makes assumptions about the type T.
// Default constructor of T is its null form T(0)
// T(0) * T = 0
// T is commutative, associative
// T has scalar multiplication
// t + (t * -1) = 0
// Should work for floating point numbers, vec2, vec3, vec4 etc.

// It has an internal state, and should be updated consistently

template<typename T>
class PIDController
{
public:

	// Default constructor initializes all values with (0.0)
	PIDController() :
		m_goal(0.0),
		m_error(0.0),
		m_lastError(0.0),
		m_deltaError(0.0),
		m_integral(0.0),
		m_kp(0.0),
		m_ki(0.0),
		m_kd(0.0),
		m_output(0.0),
		m_gain(0.0)
	{
	}

	//! set the value the controller works to maintain
	template<typename T>
	inline void setGoal(const T& a_goal) { m_goal = a_goal; }

	//! get the value the controller works to maintain
	template<typename T>
	inline T getGoal() {return m_goal; }

	//! reset internal state members, (but not settings or the goal point)
	template<typename T>
	inline void reset() {
		m_error = T(0.0),
			m_lastError = T(0.0),
			m_deltaError = T(0.0),
			m_integral = T(0.0),
			m_output = T(0.0);
	}

	//! get the last output drive value calculated by the PID
	template<typename T>
	inline T getOutput() { return m_output * m_gain; }

	//! get the last proportional value calculated by the PID
	template<typename T>
	inline T getProportionalOutput() { return m_output * m_kp; }

	//! get the last integral drive calculated by the PID
	template<typename T>
	inline T getIntegralOutput() { return m_integral * m_ki; }

	//! get the last derivative drive calculated by the PID
	template<typename T>
	inline T getDerivativeOutput() { return m_derivative * m_kd; }

	//! call this every to update the output value received when getCorrection() is called. pass the current instrument reading of the value to control into {a_measuredValue}
	//! delta time cannot be 0
	template<typename T>
	inline void update(const T& a_measuredValue, const double& a_deltaTime)
	{
		// update for change in error
		m_error = m_goal - a_measuredValue;
		m_deltaError = m_error - m_lastError;
		m_lastError = m_error;

		///// determine new output /////

		// reset output to 0 
		m_output *= 0.0;

		// proportional
		m_output += m_kp * m_error;

		// integral 
		// adds error
		m_integral += m_error * a_deltaTime;
		m_output += m_integral * m_ki;

		// derivative 
		// smoothes changes in error
		m_derivative = (m_deltaError / std::max(a_deltaTime, DBL_EPSILON));
		m_output += m_derivative * m_kd;

		//#ifdef _DEBUG
		//if (a_deltaTime < DBL_EPSILON)
			//std::cout << "warning! deltatime was zero in PID update! Divide-by-zero would have occured, but instead divided by an arbitrary small number\n";
		//#endif
	}

	//! set the weights for each component of the PID Controller
	inline void configure(double a_kProportional, double a_kIntegral, double a_kDerivative, double a_outputGain) {
		m_kp = a_kProportional;
		m_ki = a_kIntegral;
		m_kd = a_kDerivative;
		m_gain = a_outputGain;
	}

	//! weighting of proportional component
	double m_kp;

	//! weighting of integral component
	double m_ki;

	//! weighting of derivative component
	double m_kd;

	//! scales the output
	double m_gain;

private:
	T m_goal;

	T m_error; // difference between current measured state and goal
	T m_lastError; // last calculated error in state
	T m_deltaError; // change in the error from last calculation

	T m_integral; // error accumulated over time by the integral portion
	T m_derivative; // derivative output (unscaled)

	//! the PD Controller's recommendation on what the change should be to reach the target
	T m_output;
};