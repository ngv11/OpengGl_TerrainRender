#version 330

uniform sampler2D heightMap;
uniform sampler2D grassTerrain;
uniform sampler2D waterTerrain;
uniform sampler2D snowTerrain;
uniform sampler2D rockTerrain;

uniform float waterHeight;
uniform float snowHeight;


in vec2 oTexCoord;
in vec4 posn;
in float diffTerm;
in float specTerm;
in float height;
out vec4 outputColor;


void main() 
{

    vec4 texColorGrass = texture(grassTerrain, oTexCoord);
    vec4 texColorWater = texture(waterTerrain, oTexCoord);
    vec4 texColorSnow  = texture(snowTerrain, oTexCoord);
    vec4 texColorRock  = texture(rockTerrain, oTexCoord);
    vec4 out_temp;
    vec4 white = vec4(1.0);

    if (height < waterHeight)
    {
        //outputColor = vec4(0, 0, 139, 1) + (texColorWater * ((1 - (waterHeight - 2) - height))) ;
        out_temp =  (texColorWater * (1 - (waterHeight - height)*0.55)); //(vec4(0, 0, 120, 1) +
    } 
    //else if (height >= snowHeight)
    //{
    //    out_temp = texColorSnow;
    //}
    //else if (height >= (snowHeight - 1.2) && height < snowHeight)
    //{
    //    out_temp = texColorRock * (snowHeight - height) + texColorSnow * (1 - (snowHeight - height));
    //}
    //else if (height >= (snowHeight - 2.1) && height < (snowHeight - 1.2))
    //{
    //    out_temp = texColorGrass * ((snowHeight - 1.2) - height) + texColorRock * (1 - ((snowHeight - 1.2) - height));
    //}
    //else
    //{
    //    out_temp = texColorGrass;
    //}

    //--------------------------------
    // looks better

    else if (height <= snowHeight - 3)
    {   
        out_temp = texColorGrass;
    }
    else if (height <= snowHeight - 2)
    {
        out_temp = texColorRock * (1 - ((snowHeight - 2) - height)) + texColorGrass * ((snowHeight - 2) - height);
    }
    else if (height <= snowHeight - 1)
    {
        out_temp = texColorRock;
    }
    else if (height <= snowHeight)
    {
        out_temp = texColorSnow * (1 - ((snowHeight) - height)) + texColorRock * ((snowHeight) - height);
    }
    else
    {
        out_temp = texColorSnow;
    }

    outputColor = out_temp * (diffTerm);
     
    // gl_FragColor = vec4(0, 0, 1, 1);
}
