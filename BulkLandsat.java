import java.io.File;

import gov.nasa.worldwind.geom.Sector;
import gov.nasa.worldwind.geom.Angle;

import gov.nasa.worldwind.layers.Earth.LandsatI3WMSLayer;

import gov.nasa.worldwind.cache.BasicDataFileStore;
import gov.nasa.worldwind.cache.FileStore;
import gov.nasa.worldwind.event.BulkRetrievalEvent;
import gov.nasa.worldwind.event.BulkRetrievalListener;
import gov.nasa.worldwind.retrieve.BulkRetrievalThread;

class BulkLandsat {

    public static void main(String[] args) {
        
        LandsatI3WMSLayer layer = new LandsatI3WMSLayer(); 
        
        double latmin =  -90;
        double latmax =   90;
        double lonmin = -180;
        double lonmax =  180;

        Sector sector = new Sector(
                Angle.fromDegrees(latmin),
                Angle.fromDegrees(latmax),
                Angle.fromDegrees(lonmin), 
                Angle.fromDegrees(lonmax));

        double resolution = 1;

        File file = new File("image_cache");

        BasicDataFileStore fileStore = new BasicDataFileStore(file);

        long estSize = layer.getEstimatedMissingDataSize(sector,
                resolution, fileStore);

        System.out.printf("Estimated size (bytes): %,d \n", estSize);

        BulkLandsatListener listener = new BulkLandsatListener();

        BulkRetrievalThread thread = layer.makeLocal(sector, resolution,
                fileStore, listener);

        thread.run();

    }

    private static class BulkLandsatListener implements BulkRetrievalListener {

         public void eventOccurred(BulkRetrievalEvent event) {
             
             System.out.printf("%s: item %s\n",
                     event.getEventType().equals(
                         BulkRetrievalEvent.RETRIEVAL_SUCCEEDED) ? "Succeeded" :
                     event.getEventType().equals(
                         BulkRetrievalEvent.RETRIEVAL_FAILED) ? "Failed" :
                     "Unknown event type", event.getItem());

        }

    }

}
