/**
 * @file Simulation/GraphicalObject.h
 * Declaration of class GraphicalObject
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore2.h"
#include "Simulation/SimObject.h"
#include <list>

class GraphicsContext;

/**
 * @class GraphicalObject
 * Abstract class for scene graph objects with graphical representation or subordinate graphical representation
 */
class GraphicalObject
{
public:
  std::list<GraphicalObject*> graphicalDrawings; /**< List of subordinate graphical scene graph objects */

  /**
   * Creates resources to later draw the object in the given graphics context
   * @param graphicsContext The graphics context to create resources in
   */
  virtual void createGraphics(GraphicsContext& graphicsContext);

  /**
   * Submits draw calls for appearance primitives of the object (including children) in the given graphics context
   * @param graphicsContext The graphics context to draw the object to
   * @param drawControllerDrawings Whether controller drawings should be drawn instead of the real appearance
   */
  virtual void drawAppearances(GraphicsContext& graphicsContext, bool drawControllerDrawings) const;

protected:
  /**
   * Registers an element as parent
   * @param element The element to register
   */
  virtual void addParent(Element& element);

  // API
  virtual bool registerDrawing(SimRobotCore2::Controller3DDrawing& drawing);
  virtual bool unregisterDrawing(SimRobotCore2::Controller3DDrawing& drawing);

private:
  std::list<SimRobotCore2::Controller3DDrawing*> controllerDrawings; /**< Drawings registered by another SimRobot module */
};
