/**
 * @file TestSimulator.h
 * @brief Defines the TestSimulator class responsible for executing
 *        integration and validation test suites for the scheduling simulator.
 */

#pragma once

 /**
  * @class TestSimulator
  * @brief Provides static methods to run end-to-end simulation tests
  *        across various scheduling algorithms and workload scenarios.
  */
class TestSimulator
{
public:
    /**
     * @brief Executes the full suite of integration tests (standard workloads,
     *        saturated loads, and priority context-switching scenarios).
     */
	static void RunSimulatorTest();

    /**
     * @brief Executes the Test scheduler switching simulator 
     */
    static void RunSimulatorSwitchTest();
};