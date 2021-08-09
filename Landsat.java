import gov.nasa.worldwind.geom.Sector;
import gov.nasa.worldwind.geom.Angle;

import gov.nasa.worldwind.layers.Earth.LandsatI3WMSLayer;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class Landsat {

//
//public static LandsatI3WMSLayer layer;

    public static LandsatI3WMSLayer layer;

    //--------------------------------------------------------------------------
    //                     Acquire Landsat-Based Image
    //
    //  Produces image of rectangular Earth sector & saves to "landsat_temp.png"
    //
    //  IN:  widp, lenp - Width & height of image (pixels)
    //       latc, lonc - Latitude & longitude of northwest corner (deg)
    //       degp       - Degrees per pixel
    //
    //  OUT: Flag (0 for success)
    //
    //--------------------------------------------------------------------------

    public static int getImage(int widp, int lenp, double latc, double lonc,
            double dp) {

        // Flag
        int flag;
        
        try {

            // Initialize Landsat image layer
            if (layer == null)
                layer = new LandsatI3WMSLayer(); 
    
            // Compute sector    
            Sector sector = new Sector(Angle.fromDegrees(latc - lenp*dp),
                                       Angle.fromDegrees(latc),
                                       Angle.fromDegrees(lonc), 
                                       Angle.fromDegrees(lonc + widp*dp));

            // Aspect ratio (pixel?)
            double ar = 1; 

            // Timeout (1 minute)
            int timeout = 60000;

            // Image type & filename 
            String type = "image/png";
            String filename = "landsat_temp.png";

            // Acquire image 
            BufferedImage img = layer.composeImageForSector(sector, widp, lenp, ar,
                    -1, type, true, null, timeout);

            // Save image
            File outputfile = new File(filename);
            ImageIO.write(img, "png", outputfile);

            // Success
            flag = 0;

        } catch (Exception e) {

            // Failure
            flag = 1; 
            e.printStackTrace(System.out);
        
        }

        return flag;

    }

}
