#include <iostream>
#include <QFile>
#include <QXmlStreamWriter>
#include "Headers/edge.h"
#include "Headers/graph.h"
#include "Headers/node.h"

// public

Graph::Graph(QGraphicsScene *scene) :
    m_scene(scene)
{

}

void Graph::addNode(Node *n)
{
    m_adjacentList[n] = QVector<Edge *>();
}

void Graph::addEdge(Edge *e)
{
    Node *key = e->nodes().first;
    m_adjacentList[key].push_back(e);
}

void Graph::saveGraph(const QString &file)
{
    writeXml(file);
}

void Graph::loadGraph(const QString &file)
{
    readXml(file);
}

QVector<Node *> Graph::nodes() const
{
    QVector<Node *> nodes;
    for(auto &iter: m_adjacentList)
    {
        nodes.push_back(iter.first);
    }
    return nodes;
}

QVector<Edge *> Graph::edges() const
{
    QVector<Edge *> edges;
    for(auto &iter: m_adjacentList)
    {
        for(auto &iter2: iter.second)
        {
            edges.push_back(iter2);
        }
    }
    return edges;
}

int Graph::numberOfNodes() const
{
    return static_cast<int>(m_adjacentList.size());
}

// private

void Graph::writeXml(const QString &file)
{
    QFile data(file);
    if(!data.open(QFile::WriteOnly | QFile::Truncate))
    {
        return;
    }

    QXmlStreamWriter xml(&data);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("Graph");

    for(auto &iter : m_adjacentList)
    {
        xml.writeStartElement("Node");

        xml.writeAttribute("value", QString::number(iter.first->value()));
        xml.writeAttribute("x",     QString::number(iter.first->pos().x()));
        xml.writeAttribute("y",     QString::number(iter.first->pos().y()));

        xml.writeEndElement();
    }

    for(auto &iter : m_adjacentList)
    {
        for(auto &iter2 : iter.second)
        {
            xml.writeStartElement("Edge");

            xml.writeAttribute("value",  QString::number(iter2->value()));
            xml.writeAttribute("first",  QString::number(iter2->nodes().first->value()));
            xml.writeAttribute("second", QString::number(iter2->nodes().second->value()));

            xml.writeEndElement();
        }
    }

    xml.writeEndElement();
    xml.writeEndDocument();

    data.close();
}

void Graph::readXml(const QString &file)
{
    QFile data(file);
    if(!data.open(QFile::ReadOnly))
    {
        return;
    }

    QXmlStreamReader xml(&data);
    while(!xml.atEnd())
    {
        xml.readNext();
        if(xml.isStartElement())
        {
            if(xml.name() == "Node")
            {
                QXmlStreamAttributes attributes = xml.attributes();

	            int value = attributes.value("value").toInt();
                qreal x = attributes.value("x").toDouble();
                qreal y = attributes.value("y").toDouble();
                QPointF pos(x, y);

                Node *newNode = new Node(value);
                newNode->setPos(pos);
                m_scene->addItem(newNode);
                addNode(newNode);
            }
            else if(xml.name() == "Edge")
            {
                QXmlStreamAttributes attributes = xml.attributes();
	            int value = attributes.value("value").toInt();
                int first = attributes.value("first").toInt();
                int second = attributes.value("second").toInt();
                Node *from = nullptr, *to = nullptr;

                for(auto &iterNode : m_adjacentList)
                {
                    if(iterNode.first->value() == first)
                    {
                        from = iterNode.first;
                    }
                    else if(iterNode.first->value() == second)
                    {
                        to = iterNode.first;
                    }
                }
                Edge *newEdge = new Edge(from, to, value);
                m_scene->addItem(newEdge);
                addEdge(newEdge);
            }
        }
    }

    data.close();
}

void Graph::AppDebug()
{
    for(auto &iter : m_adjacentList)
    {
        std::cout << "Node: "
                  << iter.first->value()
                  << std::endl
                  << "\t Edges: ";
        for(auto &iter2 : iter.second)
        {
            std::cout << iter2->nodes().second->value()
                      << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "=============================" << std::endl;
}

bool Graph::nodeComparator::operator()(Node *left, Node *right) const
{
    return left->value() < right->value();
}
