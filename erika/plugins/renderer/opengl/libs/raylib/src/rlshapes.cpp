#include "rlshapes.hpp"

#include <cfloat>  // Required for: FLT_EPSILON
#include <cmath>   // Required for: sinf(), asinf(), cosf(), acosf(), sqrtf(), fabsf()
#include <cstdlib> // Required for: RL_FREE

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
Texture2D texShapes = { 1, 1, 1, 1, 7 };             // Texture used on shapes drawing (usually a white pixel)
Rectangle texShapesRec = { 0.0f, 0.0f, 1.0f, 1.0f }; // Texture source rectangle used on shapes drawing

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static float
EaseCubicInOut(float t, float b, float c, float d); // Cubic easing

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Set texture and rectangle to be used on shapes drawing
// NOTE: It can be useful when using basic shapes and one single font,
// defining a font char white rectangle would allow drawing everything in a single draw call
void
SetShapesTexture(Texture2D texture, Rectangle source)
{
    texShapes = texture;
    texShapesRec = source;
}

// Draw a pixel
void
DrawPixel(int posX, int posY, Color color)
{
    DrawPixelV((Vector2){ (float)posX, (float)posY }, color);
}

// Draw a pixel (Vector version)
void
DrawPixelV(Vector2 position, Color color)
{
#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);

    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlColor4ub(color.r, color.g, color.b, color.a);

    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(position.x, position.y);

    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(position.x, position.y + 1);

    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(position.x + 1, position.y + 1);

    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(position.x + 1, position.y);

    rlEnd();

    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);

    rlColor4ub(color.r, color.g, color.b, color.a);

    rlVertex2f(position.x, position.y);
    rlVertex2f(position.x, position.y + 1);
    rlVertex2f(position.x + 1, position.y);

    rlVertex2f(position.x + 1, position.y);
    rlVertex2f(position.x, position.y + 1);
    rlVertex2f(position.x + 1, position.y + 1);

    rlEnd();
#endif
}

// Draw a line
void
DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f((float)startPosX, (float)startPosY);
    rlVertex2f((float)endPosX, (float)endPosY);
    rlEnd();
}

// Draw a line  (Vector version)
void
DrawLineV(Vector2 startPos, Vector2 endPos, Color color)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(startPos.x, startPos.y);
    rlVertex2f(endPos.x, endPos.y);
    rlEnd();
}

// Draw a line defining thickness
void
DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color)
{
    Vector2 delta = { endPos.x - startPos.x, endPos.y - startPos.y };
    float length = sqrtf(delta.x * delta.x + delta.y * delta.y);

    if ((length > 0) && (thick > 0)) {
        float scale = thick / (2 * length);
        Vector2 radius = { -scale * delta.y, scale * delta.x };
        Vector2 strip[4] = { { startPos.x - radius.x, startPos.y - radius.y },
                             { startPos.x + radius.x, startPos.y + radius.y },
                             { endPos.x - radius.x, endPos.y - radius.y },
                             { endPos.x + radius.x, endPos.y + radius.y } };

        DrawTriangleStrip(strip, 4, color);
    }
}

// Draw line using cubic-bezier curves in-out
void
DrawLineBezier(Vector2 startPos, Vector2 endPos, float thick, Color color)
{
    Vector2 previous = startPos;
    Vector2 current = { 0 };

    Vector2 points[2 * BEZIER_LINE_DIVISIONS + 2] = { 0 };

    for (int i = 1; i <= BEZIER_LINE_DIVISIONS; i++) {
        // Cubic easing in-out
        // NOTE: Easing is calculated only for y position value
        current.y = EaseCubicInOut((float)i, startPos.y, endPos.y - startPos.y, (float)BEZIER_LINE_DIVISIONS);
        current.x = previous.x + (endPos.x - startPos.x) / (float)BEZIER_LINE_DIVISIONS;

        float dy = current.y - previous.y;
        float dx = current.x - previous.x;
        float size = 0.5f * thick / sqrtf(dx * dx + dy * dy);

        if (i == 1) {
            points[0].x = previous.x + dy * size;
            points[0].y = previous.y - dx * size;
            points[1].x = previous.x - dy * size;
            points[1].y = previous.y + dx * size;
        }

        points[2 * i + 1].x = current.x - dy * size;
        points[2 * i + 1].y = current.y + dx * size;
        points[2 * i].x = current.x + dy * size;
        points[2 * i].y = current.y - dx * size;

        previous = current;
    }

    DrawTriangleStrip(points, 2 * BEZIER_LINE_DIVISIONS + 2, color);
}

// Draw line using quadratic bezier curves with a control point
void
DrawLineBezierQuad(Vector2 startPos, Vector2 endPos, Vector2 controlPos, float thick, Color color)
{
    const float step = 1.0f / BEZIER_LINE_DIVISIONS;

    Vector2 previous = startPos;
    Vector2 current = { 0 };
    float t = 0.0f;

    Vector2 points[2 * BEZIER_LINE_DIVISIONS + 2] = { 0 };

    for (int i = 0; i <= BEZIER_LINE_DIVISIONS; i++) {
        t = step * i;
        float a = powf(1 - t, 2);
        float b = 2 * (1 - t) * t;
        float c = powf(t, 2);

        // NOTE: The easing functions aren't suitable here because they don't take a control point
        current.y = a * startPos.y + b * controlPos.y + c * endPos.y;
        current.x = a * startPos.x + b * controlPos.x + c * endPos.x;

        float dy = current.y - previous.y;
        float dx = current.x - previous.x;
        float size = 0.5f * thick / sqrtf(dx * dx + dy * dy);

        if (i == 1) {
            points[0].x = previous.x + dy * size;
            points[0].y = previous.y - dx * size;
            points[1].x = previous.x - dy * size;
            points[1].y = previous.y + dx * size;
        }

        points[2 * i + 1].x = current.x - dy * size;
        points[2 * i + 1].y = current.y + dx * size;
        points[2 * i].x = current.x + dy * size;
        points[2 * i].y = current.y - dx * size;

        previous = current;
    }

    DrawTriangleStrip(points, 2 * BEZIER_LINE_DIVISIONS + 2, color);
}

// Draw line using cubic bezier curves with 2 control points
void
DrawLineBezierCubic(Vector2 startPos, Vector2 endPos, Vector2 startControlPos, Vector2 endControlPos, float thick, Color color)
{
    const float step = 1.0f / BEZIER_LINE_DIVISIONS;

    Vector2 previous = startPos;
    Vector2 current = { 0 };
    float t = 0.0f;

    Vector2 points[2 * BEZIER_LINE_DIVISIONS + 2] = { 0 };

    for (int i = 0; i <= BEZIER_LINE_DIVISIONS; i++) {
        t = step * i;
        float a = powf(1 - t, 3);
        float b = 3 * powf(1 - t, 2) * t;
        float c = 3 * (1 - t) * powf(t, 2);
        float d = powf(t, 3);

        current.y = a * startPos.y + b * startControlPos.y + c * endControlPos.y + d * endPos.y;
        current.x = a * startPos.x + b * startControlPos.x + c * endControlPos.x + d * endPos.x;

        float dy = current.y - previous.y;
        float dx = current.x - previous.x;
        float size = 0.5f * thick / sqrtf(dx * dx + dy * dy);

        if (i == 1) {
            points[0].x = previous.x + dy * size;
            points[0].y = previous.y - dx * size;
            points[1].x = previous.x - dy * size;
            points[1].y = previous.y + dx * size;
        }

        points[2 * i + 1].x = current.x - dy * size;
        points[2 * i + 1].y = current.y + dx * size;
        points[2 * i].x = current.x + dy * size;
        points[2 * i].y = current.y - dx * size;

        previous = current;
    }

    DrawTriangleStrip(points, 2 * BEZIER_LINE_DIVISIONS + 2, color);
}

// Draw lines sequence
void
DrawLineStrip(Vector2* points, int pointCount, Color color)
{
    if (pointCount >= 2) {
        rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 0; i < pointCount - 1; i++) {
            rlVertex2f(points[i].x, points[i].y);
            rlVertex2f(points[i + 1].x, points[i + 1].y);
        }
        rlEnd();
    }
}

// Draw a color-filled circle
void
DrawCircle(int centerX, int centerY, float radius, Color color)
{
    DrawCircleV((Vector2){ (float)centerX, (float)centerY }, radius, color);
}

// Draw a piece of a circle
void
DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color)
{
    if (radius <= 0.0f)
        radius = 0.1f; // Avoid div by zero

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle) {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle) / 90);

    if (segments < minSegments) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
        segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

        if (segments <= 0)
            segments = minSegments;
    }

    float stepLength = (endAngle - startAngle) / (float)segments;
    float angle = startAngle;

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    // NOTE: Every QUAD actually represents two segments
    for (int i = 0; i < segments / 2; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x, center.y);

        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);

        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * radius, center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
        );

        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength * 2)) * radius,
            center.y + cosf(DEG2RAD * (angle + stepLength * 2)) * radius
        );

        angle += (stepLength * 2);
    }

    // NOTE: In case number of segments is odd, we add one last piece to the cake
    if (segments % 2) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x, center.y);

        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);

        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * radius, center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
        );

        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x, center.y);
    }
    rlEnd();

    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x, center.y);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * radius, center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
        );

        angle += stepLength;
    }
    rlEnd();
#endif
}

// Draw a piece of a circle outlines
void
DrawCircleSectorLines(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color)
{
    if (radius <= 0.0f)
        radius = 0.1f; // Avoid div by zero issue

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle) {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle) / 90);

    if (segments < minSegments) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
        segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

        if (segments <= 0)
            segments = minSegments;
    }

    float stepLength = (endAngle - startAngle) / (float)segments;
    float angle = startAngle;
    bool showCapLines = true;

    rlBegin(RL_LINES);
    if (showCapLines) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(center.x, center.y);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
    }

    for (int i = 0; i < segments; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * radius, center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
        );

        angle += stepLength;
    }

    if (showCapLines) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(center.x, center.y);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
    }
    rlEnd();
}

// Draw a gradient-filled circle
// NOTE: Gradient goes from center (color1) to border (color2)
void
DrawCircleGradient(int centerX, int centerY, float radius, Color color1, Color color2)
{
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < 360; i += 10) {
        rlColor4ub(color1.r, color1.g, color1.b, color1.a);
        rlVertex2f((float)centerX, (float)centerY);
        rlColor4ub(color2.r, color2.g, color2.b, color2.a);
        rlVertex2f((float)centerX + sinf(DEG2RAD * i) * radius, (float)centerY + cosf(DEG2RAD * i) * radius);
        rlColor4ub(color2.r, color2.g, color2.b, color2.a);
        rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * radius, (float)centerY + cosf(DEG2RAD * (i + 10)) * radius);
    }
    rlEnd();
}

// Draw a color-filled circle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
void
DrawCircleV(Vector2 center, float radius, Color color)
{
    DrawCircleSector(center, radius, 0, 360, 36, color);
}

// Draw circle outline
void
DrawCircleLines(int centerX, int centerY, float radius, Color color)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
    for (int i = 0; i < 360; i += 10) {
        rlVertex2f(centerX + sinf(DEG2RAD * i) * radius, centerY + cosf(DEG2RAD * i) * radius);
        rlVertex2f(centerX + sinf(DEG2RAD * (i + 10)) * radius, centerY + cosf(DEG2RAD * (i + 10)) * radius);
    }
    rlEnd();
}

// Draw ellipse
void
DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, Color color)
{
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < 360; i += 10) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f((float)centerX, (float)centerY);
        rlVertex2f((float)centerX + sinf(DEG2RAD * i) * radiusH, (float)centerY + cosf(DEG2RAD * i) * radiusV);
        rlVertex2f((float)centerX + sinf(DEG2RAD * (i + 10)) * radiusH, (float)centerY + cosf(DEG2RAD * (i + 10)) * radiusV);
    }
    rlEnd();
}

// Draw ellipse outline
void
DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, Color color)
{
    rlBegin(RL_LINES);
    for (int i = 0; i < 360; i += 10) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(centerX + sinf(DEG2RAD * i) * radiusH, centerY + cosf(DEG2RAD * i) * radiusV);
        rlVertex2f(centerX + sinf(DEG2RAD * (i + 10)) * radiusH, centerY + cosf(DEG2RAD * (i + 10)) * radiusV);
    }
    rlEnd();
}

// Draw ring
void
DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color)
{
    if (startAngle == endAngle)
        return;

    // Function expects (outerRadius > innerRadius)
    if (outerRadius < innerRadius) {
        float tmp = outerRadius;
        outerRadius = innerRadius;
        innerRadius = tmp;

        if (outerRadius <= 0.0f)
            outerRadius = 0.1f;
    }

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle) {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle) / 90);

    if (segments < minSegments) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / outerRadius, 2) - 1);
        segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

        if (segments <= 0)
            segments = minSegments;
    }

    // Not a ring
    if (innerRadius <= 0.0f) {
        DrawCircleSector(center, outerRadius, startAngle, endAngle, segments, color);
        return;
    }

    float stepLength = (endAngle - startAngle) / (float)segments;
    float angle = startAngle;

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    for (int i = 0; i < segments; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);

        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);

        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
        );

        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
        );

        angle += stepLength;
    }
    rlEnd();

    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
        );

        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
        );
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
        );

        angle += stepLength;
    }
    rlEnd();
#endif
}

// Draw ring outline
void
DrawRingLines(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, Color color)
{
    if (startAngle == endAngle)
        return;

    // Function expects (outerRadius > innerRadius)
    if (outerRadius < innerRadius) {
        float tmp = outerRadius;
        outerRadius = innerRadius;
        innerRadius = tmp;

        if (outerRadius <= 0.0f)
            outerRadius = 0.1f;
    }

    // Function expects (endAngle > startAngle)
    if (endAngle < startAngle) {
        // Swap values
        float tmp = startAngle;
        startAngle = endAngle;
        endAngle = tmp;
    }

    int minSegments = (int)ceilf((endAngle - startAngle) / 90);

    if (segments < minSegments) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / outerRadius, 2) - 1);
        segments = (int)((endAngle - startAngle) * ceilf(2 * PI / th) / 360);

        if (segments <= 0)
            segments = minSegments;
    }

    if (innerRadius <= 0.0f) {
        DrawCircleSectorLines(center, outerRadius, startAngle, endAngle, segments, color);
        return;
    }

    float stepLength = (endAngle - startAngle) / (float)segments;
    float angle = startAngle;
    bool showCapLines = true;

    rlBegin(RL_LINES);
    if (showCapLines) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
    }

    for (int i = 0; i < segments; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
        );

        rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
            center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
        );

        angle += stepLength;
    }

    if (showCapLines) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
        rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
    }
    rlEnd();
}

// Draw a color-filled rectangle
void
DrawRectangle(int posX, int posY, int width, int height, Color color)
{
    DrawRectangleV((Vector2){ (float)posX, (float)posY }, (Vector2){ (float)width, (float)height }, color);
}

// Draw a color-filled rectangle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
void
DrawRectangleV(Vector2 position, Vector2 size, Color color)
{
    DrawRectanglePro((Rectangle){ position.x, position.y, size.x, size.y }, (Vector2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle
void
DrawRectangleRec(Rectangle rec, Color color)
{
    DrawRectanglePro(rec, (Vector2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle with pro parameters
void
DrawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color)
{
    Vector2 topLeft = { 0 };
    Vector2 topRight = { 0 };
    Vector2 bottomLeft = { 0 };
    Vector2 bottomRight = { 0 };

    // Only calculate rotation if needed
    if (rotation == 0.0f) {
        float x = rec.x - origin.x;
        float y = rec.y - origin.y;
        topLeft = (Vector2){ x, y };
        topRight = (Vector2){ x + rec.width, y };
        bottomLeft = (Vector2){ x, y + rec.height };
        bottomRight = (Vector2){ x + rec.width, y + rec.height };
    }
    else {
        float sinRotation = sinf(rotation * DEG2RAD);
        float cosRotation = cosf(rotation * DEG2RAD);
        float x = rec.x;
        float y = rec.y;
        float dx = -origin.x;
        float dy = -origin.y;

        topLeft.x = x + dx * cosRotation - dy * sinRotation;
        topLeft.y = y + dx * sinRotation + dy * cosRotation;

        topRight.x = x + (dx + rec.width) * cosRotation - dy * sinRotation;
        topRight.y = y + (dx + rec.width) * sinRotation + dy * cosRotation;

        bottomLeft.x = x + dx * cosRotation - (dy + rec.height) * sinRotation;
        bottomLeft.y = y + dx * sinRotation + (dy + rec.height) * cosRotation;

        bottomRight.x = x + (dx + rec.width) * cosRotation - (dy + rec.height) * sinRotation;
        bottomRight.y = y + (dx + rec.width) * sinRotation + (dy + rec.height) * cosRotation;
    }

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);

    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlColor4ub(color.r, color.g, color.b, color.a);

    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(topLeft.x, topLeft.y);

    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(bottomLeft.x, bottomLeft.y);

    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(bottomRight.x, bottomRight.y);

    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(topRight.x, topRight.y);

    rlEnd();

    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);

    rlColor4ub(color.r, color.g, color.b, color.a);

    rlVertex2f(topLeft.x, topLeft.y);
    rlVertex2f(bottomLeft.x, bottomLeft.y);
    rlVertex2f(topRight.x, topRight.y);

    rlVertex2f(topRight.x, topRight.y);
    rlVertex2f(bottomLeft.x, bottomLeft.y);
    rlVertex2f(bottomRight.x, bottomRight.y);

    rlEnd();
#endif
}

// Draw a vertical-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
void
DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2)
{
    DrawRectangleGradientEx(
        (Rectangle){ (float)posX, (float)posY, (float)width, (float)height }, color1, color2, color2, color1
    );
}

// Draw a horizontal-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
void
DrawRectangleGradientH(int posX, int posY, int width, int height, Color color1, Color color2)
{
    DrawRectangleGradientEx(
        (Rectangle){ (float)posX, (float)posY, (float)width, (float)height }, color1, color1, color2, color2
    );
}

// Draw a gradient-filled rectangle
// NOTE: Colors refer to corners, starting at top-lef corner and counter-clockwise
void
DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4)
{
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    rlNormal3f(0.0f, 0.0f, 1.0f);

    // NOTE: Default raylib font character 95 is a white square
    rlColor4ub(col1.r, col1.g, col1.b, col1.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(rec.x, rec.y);

    rlColor4ub(col2.r, col2.g, col2.b, col2.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(rec.x, rec.y + rec.height);

    rlColor4ub(col3.r, col3.g, col3.b, col3.a);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(rec.x + rec.width, rec.y + rec.height);

    rlColor4ub(col4.r, col4.g, col4.b, col4.a);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(rec.x + rec.width, rec.y);
    rlEnd();

    rlSetTexture(0);
}

// Draw rectangle outline
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
void
DrawRectangleLines(int posX, int posY, int width, int height, Color color)
{
#if defined(SUPPORT_QUADS_DRAW_MODE)
    DrawRectangle(posX, posY, width, 1, color);
    DrawRectangle(posX + width - 1, posY + 1, 1, height - 2, color);
    DrawRectangle(posX, posY + height - 1, width, 1, color);
    DrawRectangle(posX, posY + 1, 1, height - 2, color);
#else
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(posX + 1, posY + 1);
    rlVertex2f(posX + width, posY + 1);

    rlVertex2f(posX + width, posY + 1);
    rlVertex2f(posX + width, posY + height);

    rlVertex2f(posX + width, posY + height);
    rlVertex2f(posX + 1, posY + height);

    rlVertex2f(posX + 1, posY + height);
    rlVertex2f(posX + 1, posY + 1);
    rlEnd();
#endif
}

// Draw rectangle outline with extended parameters
void
DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color)
{
    if ((lineThick > rec.width) || (lineThick > rec.height)) {
        if (rec.width > rec.height)
            lineThick = rec.height / 2;
        else if (rec.width < rec.height)
            lineThick = rec.width / 2;
    }

    // When rec = { x, y, 8.0f, 6.0f } and lineThick = 2, the following
    // four rectangles are drawn ([T]op, [B]ottom, [L]eft, [R]ight):
    //
    //   TTTTTTTT
    //   TTTTTTTT
    //   LL    RR
    //   LL    RR
    //   BBBBBBBB
    //   BBBBBBBB
    //

    Rectangle top = { rec.x, rec.y, rec.width, lineThick };
    Rectangle bottom = { rec.x, rec.y - lineThick + rec.height, rec.width, lineThick };
    Rectangle left = { rec.x, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };
    Rectangle right = { rec.x - lineThick + rec.width, rec.y + lineThick, lineThick, rec.height - lineThick * 2.0f };

    DrawRectangleRec(top, color);
    DrawRectangleRec(bottom, color);
    DrawRectangleRec(left, color);
    DrawRectangleRec(right, color);
}

// Draw rectangle with rounded edges
void
DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color)
{
    // Not a rounded rectangle
    if ((roundness <= 0.0f) || (rec.width < 1) || (rec.height < 1)) {
        DrawRectangleRec(rec, color);
        return;
    }

    if (roundness >= 1.0f)
        roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height) ? (rec.height * roundness) / 2 : (rec.width * roundness) / 2;
    if (radius <= 0.0f)
        return;

    // Calculate number of segments to use for the corners
    if (segments < 4) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
        segments = (int)(ceilf(2 * PI / th) / 4.0f);
        if (segments <= 0)
            segments = 4;
    }

    float stepLength = 90.0f / (float)segments;

    /*
    Quick sketch to make sense of all of this,
    there are 9 parts to draw, also mark the 12 points we'll use

          point0____________________point1
          /|                    |\
         /1|          2         |3\
     point7 /__|____________________|__\ point2
       |   |point8                point9|   |
       | 8 |          9         | 4 |
       | __|____________________|__ |
     point6 \  |point11              point10|  / point3
         \7|          6         |5/
          \|____________________|/
          point5                    point4
    */
    // Coordinates of the 12 points that define the rounded rect
    const Vector2 point[12] = {
        { (float)rec.x + radius, rec.y },
        { (float)(rec.x + rec.width) - radius, rec.y },
        { rec.x + rec.width, (float)rec.y + radius }, // PO, point1, point2
        { rec.x + rec.width, (float)(rec.y + rec.height) - radius },
        { (float)(rec.x + rec.width) - radius, rec.y + rec.height }, // point3, point4
        { (float)rec.x + radius, rec.y + rec.height },
        { rec.x, (float)(rec.y + rec.height) - radius },
        { rec.x, (float)rec.y + radius }, // point5, point6, point7
        { (float)rec.x + radius, (float)rec.y + radius },
        { (float)(rec.x + rec.width) - radius, (float)rec.y + radius }, // point8, point9
        { (float)(rec.x + rec.width) - radius, (float)(rec.y + rec.height) - radius },
        { (float)rec.x + radius, (float)(rec.y + rec.height) - radius } // point10, point11
    };

    const Vector2 centers[4] = { point[8], point[9], point[10], point[11] };
    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    // Draw all the 4 corners: [1] Upper Left Corner, [3] Upper Right Corner, [5] Lower Right Corner, [7] Lower Left Corner
    for (int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
    {
        float angle = angles[k];
        const Vector2 center = centers[k];

        // NOTE: Every QUAD actually represents two segments
        for (int i = 0; i < segments / 2; i++) {
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(center.x, center.y);
            rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
            rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
            rlTexCoord2f(
                (texShapesRec.x + texShapesRec.width) / texShapes.width,
                (texShapesRec.y + texShapesRec.height) / texShapes.height
            );
            rlVertex2f(
                center.x + sinf(DEG2RAD * (angle + stepLength)) * radius,
                center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
            );
            rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(
                center.x + sinf(DEG2RAD * (angle + stepLength * 2)) * radius,
                center.y + cosf(DEG2RAD * (angle + stepLength * 2)) * radius
            );
            angle += (stepLength * 2);
        }

        // NOTE: In case number of segments is odd, we add one last piece to the cake
        if (segments % 2) {
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(center.x, center.y);
            rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
            rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
            rlTexCoord2f(
                (texShapesRec.x + texShapesRec.width) / texShapes.width,
                (texShapesRec.y + texShapesRec.height) / texShapes.height
            );
            rlVertex2f(
                center.x + sinf(DEG2RAD * (angle + stepLength)) * radius,
                center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
            );
            rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(center.x, center.y);
        }
    }

    // [2] Upper Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[0].x, point[0].y);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(point[8].x, point[8].y);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(point[9].x, point[9].y);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[1].x, point[1].y);

    // [4] Right Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[2].x, point[2].y);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(point[9].x, point[9].y);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(point[10].x, point[10].y);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[3].x, point[3].y);

    // [6] Bottom Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[11].x, point[11].y);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(point[5].x, point[5].y);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(point[4].x, point[4].y);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[10].x, point[10].y);

    // [8] Left Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[7].x, point[7].y);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(point[6].x, point[6].y);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(point[11].x, point[11].y);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[8].x, point[8].y);

    // [9] Middle Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[8].x, point[8].y);
    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(point[11].x, point[11].y);
    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(point[10].x, point[10].y);
    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(point[9].x, point[9].y);

    rlEnd();
    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);

    // Draw all of the 4 corners: [1] Upper Left Corner, [3] Upper Right Corner, [5] Lower Right Corner, [7] Lower Left Corner
    for (int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
    {
        float angle = angles[k];
        const Vector2 center = centers[k];
        for (int i = 0; i < segments; i++) {
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex2f(center.x, center.y);
            rlVertex2f(center.x + sinf(DEG2RAD * angle) * radius, center.y + cosf(DEG2RAD * angle) * radius);
            rlVertex2f(
                center.x + sinf(DEG2RAD * (angle + stepLength)) * radius,
                center.y + cosf(DEG2RAD * (angle + stepLength)) * radius
            );
            angle += stepLength;
        }
    }

    // [2] Upper Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point[0].x, point[0].y);
    rlVertex2f(point[8].x, point[8].y);
    rlVertex2f(point[9].x, point[9].y);
    rlVertex2f(point[1].x, point[1].y);
    rlVertex2f(point[0].x, point[0].y);
    rlVertex2f(point[9].x, point[9].y);

    // [4] Right Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point[9].x, point[9].y);
    rlVertex2f(point[10].x, point[10].y);
    rlVertex2f(point[3].x, point[3].y);
    rlVertex2f(point[2].x, point[2].y);
    rlVertex2f(point[9].x, point[9].y);
    rlVertex2f(point[3].x, point[3].y);

    // [6] Bottom Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point[11].x, point[11].y);
    rlVertex2f(point[5].x, point[5].y);
    rlVertex2f(point[4].x, point[4].y);
    rlVertex2f(point[10].x, point[10].y);
    rlVertex2f(point[11].x, point[11].y);
    rlVertex2f(point[4].x, point[4].y);

    // [8] Left Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point[7].x, point[7].y);
    rlVertex2f(point[6].x, point[6].y);
    rlVertex2f(point[11].x, point[11].y);
    rlVertex2f(point[8].x, point[8].y);
    rlVertex2f(point[7].x, point[7].y);
    rlVertex2f(point[11].x, point[11].y);

    // [9] Middle Rectangle
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point[8].x, point[8].y);
    rlVertex2f(point[11].x, point[11].y);
    rlVertex2f(point[10].x, point[10].y);
    rlVertex2f(point[9].x, point[9].y);
    rlVertex2f(point[8].x, point[8].y);
    rlVertex2f(point[10].x, point[10].y);
    rlEnd();
#endif
}

// Draw rectangle with rounded edges outline
void
DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, float lineThick, Color color)
{
    if (lineThick < 0)
        lineThick = 0;

    // Not a rounded rectangle
    if (roundness <= 0.0f) {
        DrawRectangleLinesEx(
            (Rectangle){ rec.x - lineThick, rec.y - lineThick, rec.width + 2 * lineThick, rec.height + 2 * lineThick },
            lineThick,
            color
        );
        return;
    }

    if (roundness >= 1.0f)
        roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height) ? (rec.height * roundness) / 2 : (rec.width * roundness) / 2;
    if (radius <= 0.0f)
        return;

    // Calculate number of segments to use for the corners
    if (segments < 4) {
        // Calculate the maximum angle between segments based on the error rate (usually 0.5f)
        float th = acosf(2 * powf(1 - SMOOTH_CIRCLE_ERROR_RATE / radius, 2) - 1);
        segments = (int)(ceilf(2 * PI / th) / 2.0f);
        if (segments <= 0)
            segments = 4;
    }

    float stepLength = 90.0f / (float)segments;
    const float outerRadius = radius + lineThick, innerRadius = radius;

    /*
    Quick sketch to make sense of all of this,
    marks the 16 + 4(corner centers point16-19) points we'll use

           point0 ================== point1
          // point8                point9 \\
         //                        \\
     point7 // point15                  point10 \\ point2
       ||   *point16             point17*    ||
       ||                            ||
       || point14                   point11  ||
     point6 \\  *point19             point18*   // point3
         \\                        //
          \\ point13              point12 //
           point5 ================== point4
    */
    const Vector2 point[16] = {
        { (float)rec.x + innerRadius, rec.y - lineThick },
        { (float)(rec.x + rec.width) - innerRadius, rec.y - lineThick },
        { rec.x + rec.width + lineThick, (float)rec.y + innerRadius }, // PO, point1, point2
        { rec.x + rec.width + lineThick, (float)(rec.y + rec.height) - innerRadius },
        { (float)(rec.x + rec.width) - innerRadius, rec.y + rec.height + lineThick }, // point3, point4
        { (float)rec.x + innerRadius, rec.y + rec.height + lineThick },
        { rec.x - lineThick, (float)(rec.y + rec.height) - innerRadius },
        { rec.x - lineThick, (float)rec.y + innerRadius }, // point5, point6, point7
        { (float)rec.x + innerRadius, rec.y },
        { (float)(rec.x + rec.width) - innerRadius, rec.y }, // point8, point9
        { rec.x + rec.width, (float)rec.y + innerRadius },
        { rec.x + rec.width, (float)(rec.y + rec.height) - innerRadius }, // point10, point11
        { (float)(rec.x + rec.width) - innerRadius, rec.y + rec.height },
        { (float)rec.x + innerRadius, rec.y + rec.height }, // point12, point13
        { rec.x, (float)(rec.y + rec.height) - innerRadius },
        { rec.x, (float)rec.y + innerRadius } // point14, point15
    };

    const Vector2 centers[4] = {
        { (float)rec.x + innerRadius, (float)rec.y + innerRadius },
        { (float)(rec.x + rec.width) - innerRadius, (float)rec.y + innerRadius }, // point16, point17
        { (float)(rec.x + rec.width) - innerRadius, (float)(rec.y + rec.height) - innerRadius },
        { (float)rec.x + innerRadius, (float)(rec.y + rec.height) - innerRadius } // point18, point19
    };

    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };

    if (lineThick > 1) {
#if defined(SUPPORT_QUADS_DRAW_MODE)
        rlSetTexture(texShapes.id);

        rlBegin(RL_QUADS);

        // Draw all the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const Vector2 center = centers[k];
            for (int i = 0; i < segments; i++) {
                rlColor4ub(color.r, color.g, color.b, color.a);
                rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
                rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
                rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
                rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
                rlTexCoord2f(
                    (texShapesRec.x + texShapesRec.width) / texShapes.width,
                    (texShapesRec.y + texShapesRec.height) / texShapes.height
                );
                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
                );
                rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
                );

                angle += stepLength;
            }
        }

        // Upper rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[0].x, point[0].y);
        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(point[8].x, point[8].y);
        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(point[9].x, point[9].y);
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[1].x, point[1].y);

        // Right rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[2].x, point[2].y);
        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(point[10].x, point[10].y);
        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(point[11].x, point[11].y);
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[3].x, point[3].y);

        // Lower rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[13].x, point[13].y);
        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(point[5].x, point[5].y);
        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(point[4].x, point[4].y);
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[12].x, point[12].y);

        // Left rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[15].x, point[15].y);
        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(point[7].x, point[7].y);
        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(point[6].x, point[6].y);
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(point[14].x, point[14].y);

        rlEnd();
        rlSetTexture(0);
#else
        rlBegin(RL_TRIANGLES);

        // Draw all of the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const Vector2 center = centers[k];

            for (int i = 0; i < segments; i++) {
                rlColor4ub(color.r, color.g, color.b, color.a);

                rlVertex2f(center.x + sinf(DEG2RAD * angle) * innerRadius, center.y + cosf(DEG2RAD * angle) * innerRadius);
                rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
                );

                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * innerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * innerRadius
                );
                rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
                );

                angle += stepLength;
            }
        }

        // Upper rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(point[0].x, point[0].y);
        rlVertex2f(point[8].x, point[8].y);
        rlVertex2f(point[9].x, point[9].y);
        rlVertex2f(point[1].x, point[1].y);
        rlVertex2f(point[0].x, point[0].y);
        rlVertex2f(point[9].x, point[9].y);

        // Right rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(point[10].x, point[10].y);
        rlVertex2f(point[11].x, point[11].y);
        rlVertex2f(point[3].x, point[3].y);
        rlVertex2f(point[2].x, point[2].y);
        rlVertex2f(point[10].x, point[10].y);
        rlVertex2f(point[3].x, point[3].y);

        // Lower rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(point[13].x, point[13].y);
        rlVertex2f(point[5].x, point[5].y);
        rlVertex2f(point[4].x, point[4].y);
        rlVertex2f(point[12].x, point[12].y);
        rlVertex2f(point[13].x, point[13].y);
        rlVertex2f(point[4].x, point[4].y);

        // Left rectangle
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex2f(point[7].x, point[7].y);
        rlVertex2f(point[6].x, point[6].y);
        rlVertex2f(point[14].x, point[14].y);
        rlVertex2f(point[15].x, point[15].y);
        rlVertex2f(point[7].x, point[7].y);
        rlVertex2f(point[14].x, point[14].y);
        rlEnd();
#endif
    }
    else {
        // Use LINES to draw the outline
        rlBegin(RL_LINES);

        // Draw all the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const Vector2 center = centers[k];

            for (int i = 0; i < segments; i++) {
                rlColor4ub(color.r, color.g, color.b, color.a);
                rlVertex2f(center.x + sinf(DEG2RAD * angle) * outerRadius, center.y + cosf(DEG2RAD * angle) * outerRadius);
                rlVertex2f(
                    center.x + sinf(DEG2RAD * (angle + stepLength)) * outerRadius,
                    center.y + cosf(DEG2RAD * (angle + stepLength)) * outerRadius
                );
                angle += stepLength;
            }
        }

        // And now the remaining 4 lines
        for (int i = 0; i < 8; i += 2) {
            rlColor4ub(color.r, color.g, color.b, color.a);
            rlVertex2f(point[i].x, point[i].y);
            rlVertex2f(point[i + 1].x, point[i + 1].y);
        }

        rlEnd();
    }
}

// Draw a triangle
// NOTE: Vertex must be provided in counter-clockwise order
void
DrawTriangle(Vector2 vec1, Vector2 vec2, Vector2 vec3, Color color)
{
#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);

    rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(vec1.x, vec1.y);

    rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
    rlVertex2f(vec2.x, vec2.y);

    rlTexCoord2f(
        (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
    );
    rlVertex2f(vec2.x, vec2.y);

    rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
    rlVertex2f(vec3.x, vec3.y);
    rlEnd();

    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(vec1.x, vec1.y);
    rlVertex2f(vec2.x, vec2.y);
    rlVertex2f(vec3.x, vec3.y);
    rlEnd();
#endif
}

// Draw a triangle using lines
// NOTE: Vertex must be provided in counter-clockwise order
void
DrawTriangleLines(Vector2 vec1, Vector2 vec2, Vector2 vec3, Color color)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(vec1.x, vec1.y);
    rlVertex2f(vec2.x, vec2.y);

    rlVertex2f(vec2.x, vec2.y);
    rlVertex2f(vec3.x, vec3.y);

    rlVertex2f(vec3.x, vec3.y);
    rlVertex2f(vec1.x, vec1.y);
    rlEnd();
}

// Draw a triangle fan defined by points
// NOTE: First vertex provided is the center, shared by all triangles
// By default, following vertex should be provided in counter-clockwise order
void
DrawTriangleFan(Vector2* points, int pointCount, Color color)
{
    if (pointCount >= 3) {
        rlSetTexture(texShapes.id);
        rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 1; i < pointCount - 1; i++) {
            rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(points[0].x, points[0].y);

            rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
            rlVertex2f(points[i].x, points[i].y);

            rlTexCoord2f(
                (texShapesRec.x + texShapesRec.width) / texShapes.width,
                (texShapesRec.y + texShapesRec.height) / texShapes.height
            );
            rlVertex2f(points[i + 1].x, points[i + 1].y);

            rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
            rlVertex2f(points[i + 1].x, points[i + 1].y);
        }
        rlEnd();
        rlSetTexture(0);
    }
}

// Draw a triangle strip defined by points
// NOTE: Every new vertex connects with previous two
void
DrawTriangleStrip(Vector2* points, int pointCount, Color color)
{
    if (pointCount >= 3) {
        rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 2; i < pointCount; i++) {
            if ((i % 2) == 0) {
                rlVertex2f(points[i].x, points[i].y);
                rlVertex2f(points[i - 2].x, points[i - 2].y);
                rlVertex2f(points[i - 1].x, points[i - 1].y);
            }
            else {
                rlVertex2f(points[i].x, points[i].y);
                rlVertex2f(points[i - 1].x, points[i - 1].y);
                rlVertex2f(points[i - 2].x, points[i - 2].y);
            }
        }
        rlEnd();
    }
}

// Draw a regular polygon of n sides (Vector version)
void
DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color)
{
    if (sides < 3)
        sides = 3;
    float centralAngle = rotation;

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    for (int i = 0; i < sides; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x, center.y);

        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);

        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);

        centralAngle += 360.0f / (float)sides;
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);
    }
    rlEnd();
    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < sides; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x, center.y);
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);

        centralAngle += 360.0f / (float)sides;
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);
    }
    rlEnd();
#endif
}

// Draw a polygon outline of n sides
void
DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color)
{
    if (sides < 3)
        sides = 3;
    float centralAngle = rotation;

    rlBegin(RL_LINES);
    for (int i = 0; i < sides; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);
        centralAngle += 360.0f / (float)sides;
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);
    }
    rlEnd();
}

void
DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float lineThick, Color color)
{
    if (sides < 3)
        sides = 3;
    float centralAngle = rotation;
    float exteriorAngle = 360.0f / (float)sides;
    float innerRadius = radius - (lineThick * cosf(DEG2RAD * exteriorAngle / 2.0f));

#if defined(SUPPORT_QUADS_DRAW_MODE)
    rlSetTexture(texShapes.id);

    rlBegin(RL_QUADS);
    for (int i = 0; i < sides; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);

        rlTexCoord2f(texShapesRec.x / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(
            center.x + sinf(DEG2RAD * centralAngle) * innerRadius, center.y + cosf(DEG2RAD * centralAngle) * innerRadius
        );

        rlTexCoord2f(texShapesRec.x / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);

        centralAngle += exteriorAngle;
        rlTexCoord2f((texShapesRec.x + texShapesRec.width) / texShapes.width, texShapesRec.y / texShapes.height);
        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);

        rlTexCoord2f(
            (texShapesRec.x + texShapesRec.width) / texShapes.width, (texShapesRec.y + texShapesRec.height) / texShapes.height
        );
        rlVertex2f(
            center.x + sinf(DEG2RAD * centralAngle) * innerRadius, center.y + cosf(DEG2RAD * centralAngle) * innerRadius
        );
    }
    rlEnd();
    rlSetTexture(0);
#else
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < sides; i++) {
        rlColor4ub(color.r, color.g, color.b, color.a);
        float nextAngle = centralAngle + exteriorAngle;

        rlVertex2f(center.x + sinf(DEG2RAD * centralAngle) * radius, center.y + cosf(DEG2RAD * centralAngle) * radius);
        rlVertex2f(
            center.x + sinf(DEG2RAD * centralAngle) * innerRadius, center.y + cosf(DEG2RAD * centralAngle) * innerRadius
        );
        rlVertex2f(center.x + sinf(DEG2RAD * nextAngle) * radius, center.y + cosf(DEG2RAD * nextAngle) * radius);

        rlVertex2f(
            center.x + sinf(DEG2RAD * centralAngle) * innerRadius, center.y + cosf(DEG2RAD * centralAngle) * innerRadius
        );
        rlVertex2f(center.x + sinf(DEG2RAD * nextAngle) * radius, center.y + cosf(DEG2RAD * nextAngle) * radius);
        rlVertex2f(center.x + sinf(DEG2RAD * nextAngle) * innerRadius, center.y + cosf(DEG2RAD * nextAngle) * innerRadius);

        centralAngle = nextAngle;
    }
    rlEnd();
#endif
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Collision Detection functions
//----------------------------------------------------------------------------------

// Check if point is inside rectangle
bool
CheckCollisionPointRec(Vector2 point, Rectangle rec)
{
    bool collision = false;

    if ((point.x >= rec.x) && (point.x <= (rec.x + rec.width)) && (point.y >= rec.y) && (point.y <= (rec.y + rec.height)))
        collision = true;

    return collision;
}

// Check if point is inside circle
bool
CheckCollisionPointCircle(Vector2 point, Vector2 center, float radius)
{
    bool collision = false;

    collision = CheckCollisionCircles(point, 0, center, radius);

    return collision;
}

// Check if point is inside a triangle defined by three points (point1, point2, point3)
bool
CheckCollisionPointTriangle(Vector2 point, Vector2 point1, Vector2 point2, Vector2 point3)
{
    bool collision = false;

    float alpha = ((point2.y - point3.y) * (point.x - point3.x) + (point3.x - point2.x) * (point.y - point3.y)) /
                  ((point2.y - point3.y) * (point1.x - point3.x) + (point3.x - point2.x) * (point1.y - point3.y));

    float beta = ((point3.y - point1.y) * (point.x - point3.x) + (point1.x - point3.x) * (point.y - point3.y)) /
                 ((point2.y - point3.y) * (point1.x - point3.x) + (point3.x - point2.x) * (point1.y - point3.y));

    float gamma = 1.0f - alpha - beta;

    if ((alpha > 0) && (beta > 0) && (gamma > 0))
        collision = true;

    return collision;
}

// Check if point is within a polygon described by array of vertices
// NOTE: Based on http://jeffreythompson.org/collision-detection/poly-point.php
bool
CheckCollisionPointPoly(Vector2 point, Vector2* points, int pointCount)
{
    bool collision = false;

    if (pointCount > 2) {
        for (int i = 0; i < pointCount - 1; i++) {
            Vector2 vc = points[i];
            Vector2 vn = points[i + 1];

            if ((((vc.y >= point.y) && (vn.y < point.y)) || ((vc.y < point.y) && (vn.y >= point.y))) &&
                (point.x < ((vn.x - vc.x) * (point.y - vc.y) / (vn.y - vc.y) + vc.x)))
                collision = !collision;
        }
    }

    return collision;
}

// Check collision between two rectangles
bool
CheckCollisionRecs(Rectangle rec1, Rectangle rec2)
{
    bool collision = false;

    if ((rec1.x < (rec2.x + rec2.width) && (rec1.x + rec1.width) > rec2.x) &&
        (rec1.y < (rec2.y + rec2.height) && (rec1.y + rec1.height) > rec2.y))
        collision = true;

    return collision;
}

// Check collision between two circles
bool
CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
{
    bool collision = false;

    float dx = center2.x - center1.x; // X distance between centers
    float dy = center2.y - center1.y; // Y distance between centers

    float distance = sqrtf(dx * dx + dy * dy); // Distance between centers

    if (distance <= (radius1 + radius2))
        collision = true;

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool
CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec)
{
    bool collision = false;

    int recCenterX = (int)(rec.x + rec.width / 2.0f);
    int recCenterY = (int)(rec.y + rec.height / 2.0f);

    float dx = fabsf(center.x - (float)recCenterX);
    float dy = fabsf(center.y - (float)recCenterY);

    if (dx > (rec.width / 2.0f + radius)) {
        return false;
    }
    if (dy > (rec.height / 2.0f + radius)) {
        return false;
    }

    if (dx <= (rec.width / 2.0f)) {
        return true;
    }
    if (dy <= (rec.height / 2.0f)) {
        return true;
    }

    float cornerDistanceSq =
        (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) + (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    collision = (cornerDistanceSq <= (radius * radius));

    return collision;
}

// Check the collision between two lines defined by two points each, returns collision point by reference
bool
CheckCollisionLines(Vector2 startPos1, Vector2 endPos1, Vector2 startPos2, Vector2 endPos2, Vector2* collisionPoint)
{
    bool collision = false;

    float div = (endPos2.y - startPos2.y) * (endPos1.x - startPos1.x) - (endPos2.x - startPos2.x) * (endPos1.y - startPos1.y);

    if (fabsf(div) >= FLT_EPSILON) {
        collision = true;

        float xi = ((startPos2.x - endPos2.x) * (startPos1.x * endPos1.y - startPos1.y * endPos1.x) -
                    (startPos1.x - endPos1.x) * (startPos2.x * endPos2.y - startPos2.y * endPos2.x)) /
                   div;
        float yi = ((startPos2.y - endPos2.y) * (startPos1.x * endPos1.y - startPos1.y * endPos1.x) -
                    (startPos1.y - endPos1.y) * (startPos2.x * endPos2.y - startPos2.y * endPos2.x)) /
                   div;

        if (((fabsf(startPos1.x - endPos1.x) > FLT_EPSILON) &&
             (xi < fminf(startPos1.x, endPos1.x) || (xi > fmaxf(startPos1.x, endPos1.x)))) ||
            ((fabsf(startPos2.x - endPos2.x) > FLT_EPSILON) &&
             (xi < fminf(startPos2.x, endPos2.x) || (xi > fmaxf(startPos2.x, endPos2.x)))) ||
            ((fabsf(startPos1.y - endPos1.y) > FLT_EPSILON) &&
             (yi < fminf(startPos1.y, endPos1.y) || (yi > fmaxf(startPos1.y, endPos1.y)))) ||
            ((fabsf(startPos2.y - endPos2.y) > FLT_EPSILON) &&
             (yi < fminf(startPos2.y, endPos2.y) || (yi > fmaxf(startPos2.y, endPos2.y)))))
            collision = false;

        if (collision && (collisionPoint != 0)) {
            collisionPoint->x = xi;
            collisionPoint->y = yi;
        }
    }

    return collision;
}

// Check if point belongs to line created between two points [point1] and [point2] with defined margin in pixels [threshold]
bool
CheckCollisionPointLine(Vector2 point, Vector2 point1, Vector2 point2, int threshold)
{
    bool collision = false;

    float dxc = point.x - point1.x;
    float dyc = point.y - point1.y;
    float dxl = point2.x - point1.x;
    float dyl = point2.y - point1.y;
    float cross = dxc * dyl - dyc * dxl;

    if (fabsf(cross) < (threshold * fmaxf(fabsf(dxl), fabsf(dyl)))) {
        if (fabsf(dxl) >= fabsf(dyl))
            collision =
                (dxl > 0) ? ((point1.x <= point.x) && (point.x <= point2.x)) : ((point2.x <= point.x) && (point.x <= point1.x));
        else
            collision =
                (dyl > 0) ? ((point1.y <= point.y) && (point.y <= point2.y)) : ((point2.y <= point.y) && (point.y <= point1.y));
    }

    return collision;
}

// Get collision rectangle for two rectangles collision
Rectangle
GetCollisionRec(Rectangle rec1, Rectangle rec2)
{
    Rectangle rec = { 0, 0, 0, 0 };

    if (CheckCollisionRecs(rec1, rec2)) {
        float dxx = fabsf(rec1.x - rec2.x);
        float dyy = fabsf(rec1.y - rec2.y);

        if (rec1.x <= rec2.x) {
            if (rec1.y <= rec2.y) {
                rec.x = rec2.x;
                rec.y = rec2.y;
                rec.width = rec1.width - dxx;
                rec.height = rec1.height - dyy;
            }
            else {
                rec.x = rec2.x;
                rec.y = rec1.y;
                rec.width = rec1.width - dxx;
                rec.height = rec2.height - dyy;
            }
        }
        else {
            if (rec1.y <= rec2.y) {
                rec.x = rec1.x;
                rec.y = rec2.y;
                rec.width = rec2.width - dxx;
                rec.height = rec1.height - dyy;
            }
            else {
                rec.x = rec1.x;
                rec.y = rec1.y;
                rec.width = rec2.width - dxx;
                rec.height = rec2.height - dyy;
            }
        }

        if (rec1.width > rec2.width) {
            if (rec.width >= rec2.width)
                rec.width = rec2.width;
        }
        else {
            if (rec.width >= rec1.width)
                rec.width = rec1.width;
        }

        if (rec1.height > rec2.height) {
            if (rec.height >= rec2.height)
                rec.height = rec2.height;
        }
        else {
            if (rec.height >= rec1.height)
                rec.height = rec1.height;
        }
    }

    return rec;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Cubic easing in-out
// NOTE: Used by DrawLineBezier() only
static float
EaseCubicInOut(float t, float b, float c, float d)
{
    if ((t /= 0.5f * d) < 1)
        return 0.5f * c * t * t * t + b;

    t -= 2;

    return 0.5f * c * (t * t * t + 2.0f) + b;
}
