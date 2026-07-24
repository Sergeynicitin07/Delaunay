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

    triangulation = Delaunay();


    points_to_add = generateRandomDOTS(num_points, width(), height());

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(points_to_add.begin(), points_to_add.end(), g);

    current_point_index = 0;


    timer->start(50);
}

void Canvas::addNextPoint() {
    if (current_point_index < points_to_add.size()) {
        Vertex p = points_to_add[current_point_index];
       

        triangulation.turn_into(p.x, p.y);

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


    for (const auto& h_face : triangulation.history) {
        if (h_face.isdead) continue;

        int iv0 = h_face.v[0];
        int iv1 = h_face.v[1];
        int iv2 = h_face.v[2];


        if (iv0 < 3 || iv1 < 3 || iv2 < 3) {
            continue;
        }


        Vertex v0 = triangulation.dcel.vertices[iv0];
        Vertex v1 = triangulation.dcel.vertices[iv1];
        Vertex v2 = triangulation.dcel.vertices[iv2];

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

