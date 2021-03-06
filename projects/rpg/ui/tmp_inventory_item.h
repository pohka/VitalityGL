#ifndef TMP_INVENTORY_ITEM_H
#define TMP_INVENTORY_ITEM_H

#include <oak/ui/ui.h>
#include "../systems/item.h"



namespace game
{
  struct tmp_InventoryItem : public ion::UIDiv
  {
    tmp_InventoryItem(Item* item);
    void update(Item* item);
    static void onItemFocus(UINode* node);
    static void onItemUnFocus(UINode* node);
    static void onItemClick(UINode* node);
  };
}

#endif