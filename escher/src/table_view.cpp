#include <escher/table_view.h>
extern "C" {
#include <assert.h>
}

#define MIN(x,y) ((x)<(y) ? (x) : (y))

TableView::TableView(TableViewDataSource * dataSource) :
  ScrollView(&m_contentView),
  m_contentView(TableView::ContentView(dataSource))
{
  setSubview(&m_contentView, 0);
}

void TableView::layoutSubviews() {
  KDRect frame;
  frame.x = 0;
  frame.y = 0;
  frame.width = m_frame.width;
  frame.height = 9999; // FIXME: Depends on the datasource, actually...
  m_contentView.setFrame(frame);

  ScrollView::layoutSubviews();
}

void TableView::scrollToRow(int index) {
  KDPoint contentOffset;
  contentOffset.x = 0;
  contentOffset.y = 0;
  setContentOffset(contentOffset);
}

#if ESCHER_VIEW_LOGGING
const char * TableView::className() const {
  return "TableView";
}
#endif

/* TableView::ContentView */

TableView::ContentView::ContentView(TableViewDataSource * dataSource) :
  View(),
  m_dataSource(dataSource)
{
  for (int i=0; i<m_dataSource->reusableCellCount(); i++) {
    setSubview(m_dataSource->reusableCell(i), i);
  }
}

int TableView::ContentView::numberOfSubviews() const {
  return MIN(m_dataSource->numberOfCells(), numberOfDisplayableCells());
}

View * TableView::ContentView::subview(int index) {
  assert(index >= 0);
  assert(index < m_dataSource->reusableCellCount());
  return m_dataSource->reusableCell(index);
}

void TableView::ContentView::storeSubviewAtIndex(View * view, int index) {
  // Do nothing!
}

void TableView::ContentView::layoutSubviews() {
  /* Here, we want to translate the offset at which our superview is displaying
   * us into an integer offset we can use to ask cells to our data source. */
  KDCoordinate pixelScrollingOffset = m_frame.y;
  int cellOffset = pixelScrollingOffset / m_dataSource->cellHeight();

  for (int i=0; i<numberOfSubviews(); i++) {
    View * cell = subview(i);

    KDRect cellFrame;
    KDCoordinate cellHeight = m_dataSource->cellHeight();
    cellFrame.x = 0;
    cellFrame.y = (cellOffset+i)*cellHeight;
    cellFrame.width = m_frame.width;
    cellFrame.height = cellHeight;

    cell->setFrame(cellFrame);

    m_dataSource->willDisplayCellForIndex(cell, cellOffset+i);
  }
}

#if ESCHER_VIEW_LOGGING
const char * TableView::ContentView::className() const {
  return "TableView::ContentView";
}
#endif

int TableView::ContentView::numberOfDisplayableCells() const {
  int result = m_superview->bounds().height / m_dataSource->cellHeight();
  assert(result <= m_dataSource->reusableCellCount());
  return result;
}