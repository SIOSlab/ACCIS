import gov.nasa.worldwind.geom.Sector;
import gov.nasa.worldwind.geom.Angle;

import gov.nasa.worldwind.layers.Earth.LandsatI3WMSLayer;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.SocketTimeoutException;

import javax.imageio.ImageIO;

class Landsat {

    public static void main(String[] args) {
        
        LandsatI3WMSLayer layer = new LandsatI3WMSLayer(); 

        final int pix_deg = 1000;

        final int widp = pix_deg;
        final int lenp = pix_deg;

        for (int clat = 0; clat < 180; clat++) {

            for (int lon = 0; lon < 360; lon++) {

                System.out.printf("clat = %03d; lon = %03d\n", clat, lon);

                get_image(clat, lon, widp, lenp, layer);

                System.out.printf("\tAcquired\n");

            }

        }

    }

    private static void get_image(int clat, int lon, int widp, int lenp, 
            LandsatI3WMSLayer layer) {

        try {

            int flag = 1;

            while (flag != 0) {
            
                try {
   
                    int lat = 90 - clat;

                    // Compute sector    
                    Sector sector = new Sector(
                                       Angle.fromDegrees(lat),
                                       Angle.fromDegrees(lat + 1),
                                       Angle.fromDegrees(lon), 
                                       Angle.fromDegrees(lon + 1));

                    // Aspect ratio (pixel?)
                    double ar = 1; 

                    // Timeout (1 minute)
                    int timeout = 60000;

                    // Image type & filename 
                    String type = "image/png";
                    String filename = String.format(
                            "mosaic/tile_%03d_%03d.png", clat, lon);

                    // Acquire image 
                    BufferedImage img = layer.composeImageForSector(sector,
                            widp, lenp, ar, -1, type, true, null, timeout);

                    // Save image
                    File outputfile = new File(filename);
                    ImageIO.write(img, "png", outputfile);

                    // Success
                    flag = 0;

                } catch (SocketTimeoutException ste) {

                    System.out.println("\tSocket timed out -- Trying again");

                }

            }

        } catch (Exception e) {

            e.printStackTrace(System.out);
        
        }

    }

}
