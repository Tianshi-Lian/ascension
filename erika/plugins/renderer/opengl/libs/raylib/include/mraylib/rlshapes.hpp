/**********************************************************************************************
 *
 *   rshapes - Basic functions to draw 2d shapes and check collisions
 *
 *   NOTES:
 *     Shapes can be draw using 3 types of primitives: LINES, TRIANGLES and QUADS.
 *     Some functions implement two drawing options: TRIANGLES and QUADS, by default TRIANGLES
 *     are used but QUADS implementation can be selected with SUPPORT_QUADS_DRAW_MODE define
 *
 *     Some functions define texture coordinates (rlTexCoord2f()) for the shapes and use a
 *     user-provided texture with SetShapesTexture(), the pourpouse of this implementation
 *     is allowing to reduce draw calls when combined with a texture-atlas.
 *
 *     By default, raylib sets the default texture and rectangle at InitWindow()[rcore] to one
 *     white character of default font [rtext], this way, raylib text and shapes can be draw with
 *     a single draw call and it also allows users to configure it the same way with their own fonts.
 *
 *   CONFIGURATION:
 *
 *   #define SUPPORT_MODULE_RSHAPES
 *       rshapes module is included in the build
 *
 *   #define SUPPORT_QUADS_DRAW_MODE
 *       Use QUADS instead of TRIANGLES for drawing when possible. Lines-based shapes still use LINES
 *
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no event
 *   will the authors be held liable for any damages arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including commercial
 *   applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *     as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

#ifndef RAYSHAPES_H
#define RAYSHAPES_H

// Check if config flags have been externally provided on compilation line
#if !defined(EXTERNAL_CONFIG_FLAGS)
#include "config.hpp" // Defines module configuration flags
#endif

#if defined(SUPPORT_MODULE_RSHAPES)

#include "rlgl.hpp" // OpenGL abstraction layer to OpenGL 1.1, 2.1, 3.3+ or ES2
#include "rltypes.hpp"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Error rate to calculate how many segments we need to draw a smooth circle,
// taken from https://stackoverflow.com/a/2244088
#ifndef SMOOTH_CIRCLE_ERROR_RATE
#define SMOOTH_CIRCLE_ERROR_RATE 0.5f // Circle error rate
#endif
#ifndef BEZIER_LINE_DIVISIONS
#define BEZIER_LINE_DIVISIONS 24 // Bezier line divisions
#endif

// Set texture and rectangle to be used on shapes drawing
// NOTE: It can be useful when using basic shapes and one single font,
// defining a font char white rectangle would allow drawing everything in a single draw call
void
SetShapesTexture(Texture2D texture, Rectangle source); // Set texture and rectangle to be used on shapes drawing

// Basic shapes drawing functions
void
DrawPixel(int posX, int posY, Color color); // Draw a pixel
void
DrawPixelV(Vector2 position, Color color); // Draw a pixel (Vector version)
void
DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color); // Draw a line
void
DrawLineV(Vector2 startPos, Vector2 endPos, Color color); // Draw a line (Vector version)
void
DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color); // Draw a line defining thickness
void
DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color); // Draw a line using cubic-bezier curves in-out
void
DrawLineBezierQuad(
    Vector2 startPos,
    Vector2 endPos,
    Vector2 controlPos,
    float thick,
    Color color
); // Draw line using quadratic bezier curves with a control point
void
DrawLineBezierCubic(
    Vector2 startPos,
    Vector2 endPos,
    Vector2 startControlPos,
    Vector2 endControlPos,
    float thick,
    Color color
); // Draw line using cubic bezier curves with 2 control points
void
DrawLineStrip(Vector2* points, int pointCount, Color color); // Draw lines sequence
void
DrawCircle(int centerX, int centerY, float radius, Color color); // Draw a color-filled circle
void
DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color); // Draw a piece of
                                                                                                             // a circle
void
DrawCircleSectorLines(
    Vector2 center,
    float radius,
    float startAngle,
    float endAngle,
    int segments,
    Color color
); // Draw circle sector outline
void
DrawCircleGradient(int centerX, int centerY, float radius, Color color1, Color color2); // Draw a gradient-filled circle
void
DrawCircleV(Vector2 center, float radius, Color color); // Draw a color-filled circle (Vector version)
void
DrawCircleLines(int centerX, int centerY, float radius, Color color); // Draw circle outline
void
DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color); // Draw ellipse
void
DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color); // Draw ellipse outline
void
DrawRing(
    Vector2 center,
    float innerRadius,
    float outerRadius,
    float startAngle,
    float endAngle,
    int segments,
    Color color
); // Draw ring
void
DrawRingLines(
    Vector2 center,
    float innerRadius,
    float outerRadius,
    float startAngle,
    float endAngle,
    int segments,
    Color color
); // Draw ring outline
void
DrawRectangle(int posX, int posY, int width, int height, Color color); // Draw a color-filled rectangle
void
DrawRectangleV(Vector2 position, Vector2 size, Color color); // Draw a color-filled rectangle (Vector version)
void
DrawRectangleRec(Rectangle rec, Color color); // Draw a color-filled rectangle
void
DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color); // Draw a color-filled rectangle with pro
                                                                              // parameters
void
DrawRectangleGradientV(
    int posX,
    int posY,
    int width,
    int height,
    Color color1,
    Color color2
); // Draw a vertical-gradient-filled rectangle
void
DrawRectangleGradientH(
    int posX,
    int posY,
    int width,
    int height,
    Color color1,
    Color color2
); // Draw a horizontal-gradient-filled rectangle
void
DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4); // Draw a gradient-filled rectangle with
                                                                                        // custom vertex colors
void
DrawRectangleLines(int posX, int posY, int width, int height, Color color); // Draw rectangle outline
void
DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color); // Draw rectangle outline with extended parameters
void
DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color); // Draw rectangle with rounded edges
void
DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color); // Draw rectangle with
                                                                                                       // rounded edges outline
void
DrawTriangle(Vector2 vec1, Vector2 vec2, Vector2 vec3, Color color); // Draw a color-filled triangle (vertex in
                                                                     // counter-clockwise order!)
void
DrawTriangleLines(Vector2 vec1, Vector2 vec2, Vector2 vec3, Color color); // Draw triangle outline (vertex in counter-clockwise
                                                                          // order!)
void
DrawTriangleFan(
    Vector2* points,
    int pointCount,
    Color color
); // Draw a triangle fan defined by points (first vertex is the center)
void
DrawTriangleStrip(Vector2* points, int pointCount, Color color); // Draw a triangle strip defined by points
void
DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color); // Draw a regular polygon (Vector version)
void
DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color); // Draw a polygon outline of n sides
void
DrawPolyLinesEx(
    Vector2 center,
    int sides,
    float radius,
    float rotation,
    float lineThick,
    Color color
); // Draw a polygon outline of n sides with extended parameters

// Basic shapes collision detection functions
bool
CheckCollisionRecs(Rectangle rec1, Rectangle rec2); // Check collision between two rectangles
bool
CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2); // Check collision between two circles
bool
CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec); // Check collision between circle and rectangle
bool
CheckCollisionPointRec(Vector2 point, Rectangle rec); // Check if point is inside rectangle
bool
CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius); // Check if point is inside circle
bool
CheckCollisionPointTriangle(Vector2 point, Vector2 point1, Vector2 point2, Vector2 point3); // Check if point is inside a
                                                                                            // triangle
bool
CheckCollisionPointPoly(
    Vector2 point,
    Vector2* points,
    int pointCount
); // Check if point is within a polygon described by array of vertices
bool
CheckCollisionLines(
    Vector2 startPos1,
    Vector2 endPos1,
    Vector2 startPos2,
    Vector2 endPos2,
    Vector2* collisionPoint
); // Check the collision between two lines defined by two points each, returns collision point by reference
bool
CheckCollisionPointLine(
    Vector2 point,
    Vector2 point1,
    Vector2 point2,
    int threshold
); // Check if point belongs to line created between two points [point1] and [point2] with defined margin in pixels [threshold]
Rectangle
GetCollisionRec(Rectangle rec1, Rectangle rec2); // Get collision rectangle for two rectangles collision

#endif // SUPPORT_MODULE_RSHAPES

#endif
