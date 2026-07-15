#include "canvas.h"
#include <QPainter>
#include <QPen>
#include <random>
#include <algorithm>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Canvas::addNextPoint);
}

void Canvas::startAnimation(int num_points) {
    dcel.vertices.clear();
    dcel.edges.clear();
    dcel.faces.clear();

    dcel.bewilder();

    points_to_add = dcel.generateRandomDOTS(num_points, width(), height());

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(points_to_add.begin(), points_to_add.end(), g);

    current_point_index = 0;

    timer->start(50);
}

void Canvas::addNextPoint() {
    if (current_point_index < points_to_add.size()) {
        Vertex p = points_to_add[current_point_index];
        dcel.turn_into(p.x, p.y);

        current_point_index++;

        update();
    } else {
        timer->stop();
    }
}

void Canvas::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    painter.setPen(QPen(Qt::blue, 1));

    for (const auto& face : dcel.faces) {
        if (face.isdead) continue;
        if (face.inner_comp == -1) continue;

        int e0 = face.inner_comp;
        int e1 = dcel.edges[e0].next;
        int e2 = dcel.edges[e1].next;

        int iv0 = dcel.edges[e0].origin;
        int iv1 = dcel.edges[e1].origin;
        int iv2 = dcel.edges[e2].origin;

        if (iv0 < 3 || iv1 < 3 || iv2 < 3) {
            continue;
        }

        Vertex v0 = dcel.vertices[iv0];
        Vertex v1 = dcel.vertices[iv1];
        Vertex v2 = dcel.vertices[iv2];

        painter.drawLine(QPointF(v0.x, v0.y), QPointF(v1.x, v1.y));
        painter.drawLine(QPointF(v1.x, v1.y), QPointF(v2.x, v2.y));
        painter.drawLine(QPointF(v2.x, v2.y), QPointF(v0.x, v0.y));
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    for (int i = 0; i < current_point_index; ++i) {
        painter.drawEllipse(QPointF(points_to_add[i].x, points_to_add[i].y), 3, 3);
    }

    if (current_point_index > 0 && current_point_index <= points_to_add.size()) {
        painter.setBrush(Qt::green);
        painter.drawEllipse(QPointF(points_to_add[current_point_index-1].x, points_to_add[current_point_index-1].y), 5, 5);
    }
}



