#include "canvas.h"
#include <QPen>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Canvas::addNextPoint);
}

void Canvas::startTriangulation(int num_points) {
    points_to_add = generateRandomDOTS(num_points, width(), height());
    current_point_index = 0;
    is_finalized = false;

    triangulation = Delaunay();

    timer->start(100);
}

void Canvas::addNextPoint() {
    if (current_point_index < points_to_add.size()) {
        Vertex p = points_to_add[current_point_index];
        triangulation.turn_into(p.x, p.y);
        current_point_index++;
        update();
    } else {

        if (!is_finalized) {
            triangulation.finalize();
            is_finalized = true;
            update();
        }
        timer->stop();
    }
}

void Canvas::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
   

    painter.fillRect(rect(), Qt::white);

    DCEL& d = triangulation.dcel;


    painter.setPen(QPen(Qt::blue, 1));
    for (size_t i = 0; i < d.edges.size(); ++i) {
        int next_e = d.edges[i].next;
        if (next_e != -1) {
            Vertex p1 = d.vertices[d.edges[i].origin];
            Vertex p2 = d.vertices[d.edges[next_e].origin];
            painter.drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
        }
    }


    painter.setPen(QPen(Qt::black, 4));
    for (size_t i = 0; i < d.vertices.size(); ++i) {
        painter.drawPoint(QPointF(d.vertices[i].x, d.vertices[i].y));
    }
}
