#include "FontButton.h"

#include <utility>  // for move

#include "control/actions/ActionDatabase.h"       // for ActionDatabase
#include "gui/toolbarMenubar/AbstractToolItem.h"  // for AbstractToolItem
#include "util/i18n.h"                            // for _
#include "util/raii/GVariantSPtr.h"


FontButton::FontButton(std::string id, ActionDatabase& db):
        AbstractToolItem(std::move(id), Category::TOOLS), gAction(db.getAction(Action::FONT)) {}

auto FontButton::createItem(bool horizontal) -> xoj::util::WidgetSPtr {
    xoj::util::GVariantSPtr fontState(g_action_get_state(G_ACTION(gAction.get())), xoj::util::adopt);
    const char* desc = g_variant_get_string(fontState.get(), nullptr);

    GtkWidget* btn = gtk_font_button_new_with_font(desc);
    gtk_font_button_set_show_size(GTK_FONT_BUTTON(btn), true);
    gtk_font_button_set_use_font(GTK_FONT_BUTTON(btn), true);
    gtk_widget_set_tooltip_text(btn, getToolDisplayName().c_str());

    // When the user picks a font, propagate to Action::FONT state
    g_signal_connect_object(btn, "font-set", G_CALLBACK(+[](GtkFontButton* fb, gpointer actionPtr) {
                                auto* font = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(fb));
                                g_action_change_state(G_ACTION(actionPtr), g_variant_new_string(font));
                                g_free(font);
                            }),
                            gAction.get(), GConnectFlags(0));

    // Keep display in sync when font changes from another source (e.g. file open)
    g_signal_connect_object(gAction.get(), "notify::state",
                            G_CALLBACK(+[](GObject* action, GParamSpec*, gpointer btn) {
                                xoj::util::GVariantSPtr font(g_action_get_state(G_ACTION(action)), xoj::util::adopt);
                                gtk_font_chooser_set_font(GTK_FONT_CHOOSER(btn), g_variant_get_string(font.get(), nullptr));
                            }),
                            btn, GConnectFlags(0));

    GtkToolItem* it = gtk_tool_item_new();
    gtk_container_add(GTK_CONTAINER(it), btn);
    return xoj::util::WidgetSPtr(GTK_WIDGET(it), xoj::util::adopt);
}

auto FontButton::getToolDisplayName() const -> std::string { return _("Font"); }

auto FontButton::getNewToolIcon() const -> GtkWidget* {
    return gtk_image_new_from_icon_name("font-x-generic", GTK_ICON_SIZE_LARGE_TOOLBAR);
}
