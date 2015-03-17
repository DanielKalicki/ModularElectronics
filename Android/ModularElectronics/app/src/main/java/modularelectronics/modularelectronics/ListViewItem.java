package modularelectronics.modularelectronics;

import android.graphics.drawable.Drawable;

public class ListViewItem {
    public final Drawable icon;
    public final String title;
    public final String description;

    public ListViewItem(Drawable icon, String title, String description){
        this.icon = icon;
        this.title = title;
        this.description = description;
    }
}
