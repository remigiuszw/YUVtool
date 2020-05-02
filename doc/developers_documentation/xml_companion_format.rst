Xml companion format
********************

In order to conveniently store parameters necessary to display picture an xml based format has been designed. Thanks to the flexibility offered by xml, different kindd of entities can be represented and interconnected in consistent way.

Labelled elements
=================

Some element types support label-ref mechanism. Top elements of such types can have *label* attribute. Non-top elements of such types can reference the labelled top level elements with *ref* attribute. The *ref* attribute must have same value as the *label* attribute of the referenced top level element. Labels starting with "YUVtool::" are reserved for built in values. Elements with *ref* attribute must have no content and no other attributes.

Element types
=============

Picture sequence
----------------

..

    <picture_sequence> ... </picture_sequence>

Elements of this type represent a sequence of 0 or more pictures. Elements must be declared in the top level.

Picture
-------

..

    <picture> ... </picture>

Elements of this type represent a picture or, if *repeat* attribute is used, sequence of pictures with common parameters. Elements can be declared in the top level and can be labelled.

Unless *ref* attribute is used, element must contain one picture format and one data element.

Valid attributes:

* *repeat* - when used, current element represents a series of pictures with common picture format. Data is also shared, but each pictures uses consecutive part of the data. Distance between consecutive picture data is defined by picture size. This attribute can only be used when current picture is part of a sequence.

Picture format
--------------

..

    <picture_format> ... </picture_format>
    
Elements of this type represent a format of a picture. Elements can be declared in the top level and can be labelled.

Unless *ref* attribute is used, element must contain one pixel format, one picture size and one data element.

Pixel format
------------

..

    <pixel_format> ... </pixel_format>-
    
Elements of this type represent a format of pixels in a picture. Elements can be declared in the top level and can be labelled.

Unless *ref* attribute is used, element must contain one color space, one or more plane and one macropixel coding element.

Color space
-----------

..

    <color_space> ... </color_space>

Elements of this type represent color spaces. Elements can be declared in the top level and can be labelled.

Unless *ref* attribute is used, element must contain one to four component elements.

Component
---------

..

    <component> ... </component>
    
Elements of this type represent a single component of a color space. Element must contain four coefficient, one valid range and one coded range element.

Coefficient
-----------

..

    <coefficient value=... />

Elements of this type represent coefficients of a color space.

The only valid attribute is:

* *value* - real value of a coefficient. Mandatory.

Valid range
-----------

..

    <valid_range min=... max=... />

Elements of this type represent valid range for color space coefficient. This is logical range, usually <0, 1> or <-0.5, 0.5>. Both *min* and *max* are inclusive.

Valid attributes are:

* *min* - lower limit of the range. Mandatory.
* *min* - higher limit of the range. Mandatory.

Coded range
-----------

..

    <coded_range min=... max=... />

Elements of this type represent coded range for color space coefficient. Values from this range will later be multiplied by :math:`\left( 2^\mathrm{bitdepth} - 1 \right)` and rounded to nearest integer during quantization. Both min and max are inclusive.

NOTE: Multiplication by :math:`\left( 2^\mathrm{bitdepth} - 1 \right)` is different to one defined in BT.XXX standards. Those standards use multiplication by :math:`2^\mathrm{bitdepth}`, where :math:`2^\mathrm{bitdepth}` will never be a result of multiplication due to "head room" reserved in coded range.

On the other hand multiplication by `2^\mathrm{bitdepth}` could result in overflow for colorspaces utilizing full <0, 1> range.

TODO: Find BT.XXX compatible way of representing coded ranges. Most obvious is using integer ranges separate for each bitdepth, e.g. <0, 255>, <0, 1023> for full range and <16, 235>, <64, 940> for BT.XXX luma range.

Valid attributes are:

* *min* - lower limit of the range. Mandatory.
* *min* - higher limit of the range. Mandatory.

Plane
-----

..

    <plane> ... </plane>

Each plane represents the way in which a rectangular array of entries corresponding to full picture surface is organized. Single picture can be described by one or more such planes. Each plane element must contain one or more entry row elements, which together correspont to one row of macropixels.

Entry row
---------

..

    <entry_row> ... </entry_row>

Elements of this type represent rows of entries in a plane. Each entry row element must contain one or more entry elements.

Entry
-----

..

    <entry width=... />

Elements of this type represent entries in a plane.

The only valid attribute is:

* *width* - number of bits occupied by entry. Mandatory.

Macropixel coding
-----------------

..

    <macropixel_coding width=... height=... > .. </macropixel_coding>

Elements of this type represent the way in which macropixels are coded. Element must contain *width* times *height* coded pixel elements.

Valid attributes are:

* *width* - number of columns of pixels in macropixel. Mandatory.
* *height* - number of rows of pixels in macropixel. Mandatory.

Coded pixel
-----------

..

    <coded_pixel> ... </coded_pixel>

Elements of this type represent the way in which single pixel of macropixel is coded. Element must contain as many component coding elements as there are components in the color space element.

Component coding
----------------

..

    <component_coding plane_index=... row_index=... entry_index=... />

Elements of this type represent the way a single color component of a pixel is coded.

Valid attributes are:

* *plane_index* - index of plane in which component intensity value is stored. Mandatory.
* *row_index* - index of row in plane. Mandatory.
* *entry_index* - index of entry in row. Mandatory.

Picture size
------------

..

    <picture_size width=... height=... />

Elements of this type represent the size of a picture.
    
Valid attributes are:

* *width* - width of a picture. Mandatory.
* *height* - height of a picture. Mandatory.

Data
----

Elements of this type represent the source of data for a picture.
    
Examples
========

Built in pixel format and picture size
--------------------------------------

.. sourcecode:: xml

    <?xml version="1.0" encoding="UTF-8" ?>

    <picture_format label="yuv420p_vga">
        <pixel_format ref="YUVtool::yuv420p" />
        <picture_size ref="YUVtool::vga" />
    </picture_format>

    <data label="data_0">
        <raw_file name="my_yuv_file.yuv" />
    </data>

    <picture_sequence>
        <picture>
            <picture_format ref="yuv420p_vga" />
            <data ref="data_0" />
        </picture>
    </picture_sequence>

Custom pixel format and picture size
------------------------------------

.. sourcecode:: xml

    <?xml version="1.0" encoding="UTF-8" ?>
    
    <color_space>
        <component>
            <coefficient value=0.299 />
            <coefficient value=0.587 />
            <coefficient value=0.114 />
            <coefficient value=0 />
        </component>
        <component>
            <coefficient value=-0.168935 />
            <coefficient value=-0.331665 />
            <coefficient value=0.50059 />
            <coefficient value=0 />
        </component>
        <component>
            <coefficient value=0.499813 />
            <coefficient value=-0.418531 />
            <coefficient value=-0.081282 />
            <coefficient value=0 />
        </component>
    </color_space>

    <pixel_format label="yuv420_10bit_packed">
        <color_space ref="yuv">
        <plane>
            <entry_row>
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
            </entry_row>
            <entry_row>
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
            </entry_row>
        </plane>
        <plane>
            <entry_row>
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
            </entry_row>
        </plane>
        <plane>
            <entry_row>
                <entry width=10 />
                <entry width=10 />
                <entry width=10 />
                <entry width=2 />
            </entry_row>
        </plane>
        <macropixel_coding width=6 height=2>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=0 />
                <component_coding plane_index=1 row_index=0 entry_index=0 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=1 />
                <component_coding plane_index=1 row_index=0 entry_index=0 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=2 />
                <component_coding plane_index=1 row_index=0 entry_index=1 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=4 />
                <component_coding plane_index=1 row_index=0 entry_index=1 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=5 />
                <component_coding plane_index=1 row_index=0 entry_index=2 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=0 entry_index=6 />
                <component_coding plane_index=1 row_index=0 entry_index=2 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=0 />
                <component_coding plane_index=1 row_index=0 entry_index=0 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=1 />
                <component_coding plane_index=1 row_index=0 entry_index=0 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=2 />
                <component_coding plane_index=1 row_index=0 entry_index=1 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=4 />
                <component_coding plane_index=1 row_index=0 entry_index=1 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=5 />
                <component_coding plane_index=1 row_index=0 entry_index=2 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
            <coded_pixel>
                <component_coding plane_index=0 row_index=1 entry_index=6 />
                <component_coding plane_index=1 row_index=0 entry_index=2 />
                <component_coding plane_index=2 row_index=0 entry_index=0 />
            </coded_pixel>
        </macropixel_coding>
    </pixel_format>

    <data label="data_0">
        <raw_file name="my_yuv_file.yuv" />
    </data>

    <picture_sequence>
        <picture>
            <picture_format>
                <pixel_format ref="yuv420_10bit_packed"/>
                <picture_size width=1280 height=720 />
            </picture_format>
            <data ref="data_0" />
        </picture>
    </picture_sequence>
