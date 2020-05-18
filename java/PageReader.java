package dpdf;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
//import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDDocumentOutline;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDOutlineItem;
import org.apache.pdfbox.pdmodel.interactive.documentnavigation.outline.PDOutlineNode;

import org.apache.pdfbox.text.PDFTextStripper;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

public class PageReader {

	public PDDocument document;
	public PDFTextStripper reader;

	public static void main(String[] args) throws Exception{

		//		if (args.length < 1)
		//			return;

		//		String filename = "";
		//
		//		for (int i = 0; i < args.length; i++) {
		//			filename += args[i];
		//			filename += "\"";
		//		}
		//		filename = filename.substring(0, filename.length() - 1);

		PageReader pageReader = new PageReader();

		//		try {
		//			pageReader.document = PDDocument.load(new File(filename));
		//			pageReader.reader = new PDFTextStripper();
		//			//			PDDocumentOutline outline = pageReader.document.getDocumentCatalog().getDocumentOutline();
		//			//			getOutlines(pageReader.document, outline, "");
		//			System.out.print(pageReader.document.getNumberOfPages());
		//			System.out.println("@pagereader finished contents@");
		//		} catch (IOException e) {
		//			// TODO Auto-generated catch block
		//			e.printStackTrace();
		//		}

		String pages = "";

		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in)); 

		while (pages != "pages") {
			//Enter data using BufferReader 

			// Reading data using readLine 
			pages = reader.readLine();
			//			System.out.println(pages);

			if (pages.startsWith("pages"))
				break;

			if (pages.startsWith("open")) {
				String filename = pages.substring(5, pages.length());
				if (pageReader.document != null)
					pageReader.document.close();

				try {
					pageReader.document = PDDocument.load(new File(filename));
					pageReader.reader = new PDFTextStripper();
					//			PDDocumentOutline outline = pageReader.document.getDocumentCatalog().getDocumentOutline();
					//			getOutlines(pageReader.document, outline, "");
					System.out.print(pageReader.document.getNumberOfPages());
					System.out.println("@pagereader finished contents@");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

//				System.out.println(filename);
//				System.out.println("#end of page#");
			}

			if (pages.startsWith("page")) {
				String page = pages.substring(5, pages.length());
				int pageid = Integer.parseInt(page);
				pageReader.pageText(pageid);
			}

			//			System.out.println(pages); 



			// Printing the read line 

		}
		if (pageReader.document != null)
			pageReader.document.close();
	}

	void openFile(String filename) {

	}


	void pageText (int page) {
		try {
			reader.setStartPage(page);
			reader.setEndPage(page);
			String pageText = reader.getText(document);
			System.out.println(pageText);
			System.out.println("#end of page#");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
	}

	public static void getOutlines(PDDocument document, PDOutlineNode bookmark, String indentation) throws IOException {
		PDOutlineItem current = bookmark.getFirstChild();
		while (current != null) {
			PDPage currentPage = current.findDestinationPage(document);
			Integer pageNumber = document.getDocumentCatalog().getPages().indexOf(currentPage) + 1;
			System.out.println(current.getTitle() + "-------->" + pageNumber);
			getOutlines(document, current, indentation);
			current = current.getNextSibling();
		}
	}
}