#include "ui_node.h"
#include "ui_component.h"
#include "ui_canvas.h"
#include <core/input.h>
#include <debug.h>

using namespace oak::ui;
using namespace oak;

UINode::UINode(const uchar nodeType)
{
  this->nodeType = nodeType;
  computedStyle = new Style("");
  inlineStyle = new Style("self");

  
  //default style
  
}

UINode::~UINode()
{
  for (uint i = 0; i < children.size(); i++)
  {
    delete children[i];
  }
  children.clear();
}

void UINode::setComponent(UIComponent* component)
{
  this->component = component;
}

void UINode::addChild(UINode* node)
{
  node->parent = this;
  node->isRootNode = false;
  node->component = component;
  children.push_back(node);
}

UINode* UINode::getParent()
{
  return parent;
}

uchar UINode::getType()
{
  return nodeType;
}

bool UINode::getIsRootNode()
{
  return isRootNode;
}

Point& UINode::getParentPos()
{
  if (isRootNode)
  {
    return component->pos;
  }
  else
  {
    return parent->pos;
  }
}

float UINode::getTotalH()
{
  return totalH;
}

void UINode::renderEnd(Point& nodeCursor)
{
  Point childCursor = { nodeCursor.x, nodeCursor.y };

  totalW = 
    computedStyle->attrs[style::margin_left] +
    computedStyle->attrs[style::margin_right];
  if (computedStyle->attrs[style::width] != Style::NULL_ATTR)
  {
    totalW += computedStyle->attrs[style::width];
  }

  totalH = 
    computedStyle->attrs[style::margin_top] +
    computedStyle->attrs[style::margin_bottom];

  //calculate total height if automatic height
  if (computedStyle->attrs[style::height] == Style::NULL_ATTR)
  {
    //margin and padding
    //h += (ushort)(margin.y + padding.y) * 2;
    //computedStyle->attrs[style::height] = 0.0f;

    for (auto node : children)
    {
      node->render(childCursor);
      if (node->positionType == UI_POSITION_RELATIVE)
      {
       float childNodeH = node->getTotalH();
        childCursor.y -= childNodeH;
        totalH += childNodeH; //height of each child node
      }
    }
    //h = totalH;
  }
  else
  {
    totalH += computedStyle->attrs[style::height];

    for (auto node : children)
    {
      node->render(childCursor);
      if (node->positionType == UI_POSITION_RELATIVE)
      {
        childCursor.y -= node->getTotalH();
      }
    }
  }

  if (Input::hasMouseMoved())
  {
    glm::vec2 windowUnit = Window::getWindowUnitToPixel();
    rect.x = nodeCursor.x;// +windowUnit.x;
    rect.y = nodeCursor.y;
    rect.w = 200.0f;
    //  computedStyle->attrs[style::width] +
    //  computedStyle->attrs[style::padding_left] +
    //  computedStyle->attrs[style::padding_right];
    rect.h = 40.0f;
    //  computedStyle->attrs[style::height] + 
    //  computedStyle->attrs[style::padding_top] + 
    //  computedStyle->attrs[style::padding_bottom];

    float xx = (Input::mousePos.x - Window::getWidth() / 2.0f) / Window::getWindowToVPRatio().x;
    float yy = (Input::mousePos.y - Window::getHeight() / 2.0f) / Window::getWindowToVPRatio().y;

    if (onFocus != nullptr)
    {
     // LOG << "windowUnit:" << windowUnit.x << "," << windowUnit.y;
      LOG << "rect:" << rect.x << "," << rect.y << "," << rect.w << "," << rect.h;
      LOG << "mouse:" << xx << "," << yy;
      //onFocus(this);
    }

    

    if (rect.containsPt(xx, yy))
    {
      if (onFocus != nullptr)
      {
        LOG << "contains pt";
        onFocus(this);
      }
    }
  }

  Point padding = {};
  if (computedStyle->attrs[style::padding_left] != Style::NULL_ATTR)
  {
    padding.x = computedStyle->attrs[style::padding_left];
  }
  if (computedStyle->attrs[style::padding_top] != Style::NULL_ATTR)
  {
    padding.y = computedStyle->attrs[style::padding_top];
  }

  Point margin = {};
  if (computedStyle->attrs[style::margin_left] != Style::NULL_ATTR)
  {
    margin.x = computedStyle->attrs[style::margin_left];
  }
  if (computedStyle->attrs[style::margin_top] != Style::NULL_ATTR)
  {
    margin.y = computedStyle->attrs[style::margin_top];
  }

  nodeCursor.x -= padding.x + margin.x;

  nodeCursor.y -= padding.y - margin.y;
}

void UINode::renderBegin(Point& nodeCursor)
{
  Point parentPos = getParentPos();
  pos.x = parentPos.x;
  pos.y = parentPos.y;
  
  Point padding = {};
  if (computedStyle->attrs[style::padding_left] != Style::NULL_ATTR)
  {
    padding.x = computedStyle->attrs[style::padding_left];
  }
  if (computedStyle->attrs[style::padding_top] != Style::NULL_ATTR)
  {
    padding.y = computedStyle->attrs[style::padding_top];
  }

  Point margin = {};
  if (computedStyle->attrs[style::margin_left] != Style::NULL_ATTR)
  {
    margin.x = computedStyle->attrs[style::margin_left];
  }
  if (computedStyle->attrs[style::margin_top] != Style::NULL_ATTR)
  {
    margin.y = computedStyle->attrs[style::margin_top];
  }

  nodeCursor.x += padding.x + margin.x;
  nodeCursor.y += - padding.y - margin.y;
}

void UINode::addClass(Style* style)
{
  computedStyle->classList.push_back(style->classList[0]);
}

void UINode::calcStyle()
{
  for (std::string cls : computedStyle->classList)
  {
    Style* style = UICanvas::findStyle(cls);
    if (style != nullptr)
    {
      mutateComputedStyle(style);
    }
  }

  mutateComputedStyle(inlineStyle);
  if (computedStyle->attrs[style::font_size] == Style::NULL_ATTR)
  {
    computedStyle->attrs[style::font_size] = Style::DEFAULT_FONT_SIZE;
  }
}

void UINode::mutateComputedStyle(Style* style)
{
  if (computedStyle->position < style->position)
  {
    computedStyle->position = style->position;
  }

  //color is not null
  if (style->color.a >= 0.0f)
  {
    computedStyle->color = style->color;
  }

  for (uint i = 0; i < style->attrs.size(); i++)
  {
    //negative values are used
    if (style->attrs[i] != Style::NULL_ATTR)
    {
      computedStyle->attrs[i] = style->attrs[i];
    }
  }
}