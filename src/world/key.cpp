#include <world/world.hpp>


namespace MCPP {


	static const String key_template("column_{0}_{1}_{2}");


	String WorldContainer::key (ColumnID id) const {
	
		return String::Format(
			key_template,
			id.X,
			id.Z,
			id.Dimension
		);
	
	}
	
	
	String WorldContainer::key (const ColumnContainer & column) const {
	
		return key(column.ID());
	
	}


}
